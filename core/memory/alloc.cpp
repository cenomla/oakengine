#include "alloc.h"

#include "debug_vars.h"

namespace oak {
	
	Allocator::Allocator(void *start, size_t size) : start_{start}, size_{size}, usedMemory_{0}, numAllocs_{0} {}

	LinearAllocator::LinearAllocator(void *start, size_t size) : Allocator{start, size}, currentPos_{start} {}

	void* LinearAllocator::allocate(size_t size, uint32_t alignment) {
		oak_assert(size!=0);

		uint32_t adjustment = ptrutil::alignForwardAdjustment(currentPos_, alignment);

		if (usedMemory_ + adjustment + size > size_) {
			return nullptr;
		}

		uintptr_t alignedAddress = reinterpret_cast<uintptr_t>(currentPos_) + adjustment;

		currentPos_ = reinterpret_cast<void*>(alignedAddress + size);

		usedMemory_ += size + adjustment;
		numAllocs_++;

		return reinterpret_cast<void*>(alignedAddress);
	}

	void LinearAllocator::clear() {
		numAllocs_ = 0;
		usedMemory_ = 0;
		currentPos_ = start_;
	}



	StackAllocator::StackAllocator(void *start, size_t size) : Allocator{start, size}, currentPos_{ start_ }, previousPos_{ nullptr } {}

	void *StackAllocator::allocate(size_t size, uint32_t alignment) {
		oak_assert(size != 0);

		previousPos_ = currentPos_;

		uint32_t adjustment = ptrutil::alignForwardAdjustmentWithHeader(currentPos_, alignment, sizeof(AllocationHeader));

		if (usedMemory_ + adjustment + size > size_) {
			return nullptr;
		}

		void *alignedAddress = ptrutil::add(currentPos_, adjustment);

		AllocationHeader* header = static_cast<AllocationHeader*>(ptrutil::subtract(alignedAddress, sizeof(AllocationHeader)));

		header->adjustment = adjustment;
		header->prevAddress = previousPos_;

		currentPos_ = ptrutil::add(alignedAddress, size);

		usedMemory_ += size + adjustment;
		numAllocs_++;

		return alignedAddress;
	}

	void StackAllocator::deallocate(void *p) {
		if (p != previousPos_) { return; }

		AllocationHeader* header = static_cast<AllocationHeader*>(ptrutil::subtract(p, sizeof(AllocationHeader)));
		previousPos_ = header->prevAddress;

		usedMemory_ -= reinterpret_cast<uintptr_t>(currentPos_) - reinterpret_cast<uintptr_t>(p) + header->adjustment;

		currentPos_ = ptrutil::subtract(p, header->adjustment);

		numAllocs_--;

		if (numAllocs_ == 0) {
			previousPos_ = nullptr;
		}
	}

	FreelistAllocator::FreelistAllocator(void *start, size_t size) : Allocator{ start, size }, freeList_{static_cast<FreeBlock*>(start)} {
		oak_assert(size > sizeof(FreeBlock));

		freeList_->size = size;
		freeList_->next = nullptr;
	}

	void* FreelistAllocator::allocate(size_t size, uint32_t alignment) {
		FreeBlock* prev_free_block = nullptr;
		FreeBlock* free_block = freeList_;

		while (free_block != nullptr)
		{
			//Calculate adjustment needed to keep object correctly aligned
			uint32_t adjustment = ptrutil::alignForwardAdjustmentWithHeader(free_block, alignment, sizeof(AllocationHeader));

			size_t total_size = size + adjustment;

			//If allocation doesn't fit in this FreeBlock, try the next
			if (free_block->size < total_size)
			{
				prev_free_block = free_block;
				free_block = free_block->next;
				continue;
			}

			//static_assert(sizeof(AllocationHeader) >= sizeof(FreeBlock), "sizeof(AllocationHeader) < sizeof(FreeBlock)");

			//If allocations in the remaining memory will be impossible
			if (free_block->size - total_size <= sizeof(AllocationHeader))
			{
				//Increase allocation size instead of creating a new FreeBlock
				total_size = free_block->size;

				if (prev_free_block != nullptr)
					prev_free_block->next = free_block->next;
				else
					freeList_ = free_block->next;
			}
			else
			{
				//Else create a new FreeBlock containing remaining memory
				FreeBlock* next_block = static_cast<FreeBlock*>(ptrutil::add(free_block, total_size));
				next_block->size = free_block->size - total_size;
				next_block->next = free_block->next;

				if (prev_free_block != nullptr) {
					prev_free_block->next = next_block;
				}
				else {
					freeList_ = next_block;
				}
			}

			uintptr_t aligned_address = reinterpret_cast<uintptr_t>(free_block) + adjustment;

			AllocationHeader* header = (AllocationHeader*)(aligned_address - sizeof(AllocationHeader));
			header->adjustment = adjustment;

			usedMemory_ += total_size;
			numAllocs_++;

			oak_assert(ptrutil::alignForwardAdjustment((void*)aligned_address, alignment) == 0);

			return reinterpret_cast<void*>(aligned_address);
		}

		return nullptr;
	}

	void FreelistAllocator::deallocate(void *ptr, size_t size) {
		oak_assert(ptr != nullptr);

		AllocationHeader* header = static_cast<AllocationHeader*>(ptrutil::subtract(ptr, sizeof(AllocationHeader)));

		uintptr_t   block_start = reinterpret_cast<uintptr_t>(ptr) - header->adjustment;
		uintptr_t   block_end = block_start + size;

		FreeBlock* prev_free_block = nullptr;
		FreeBlock* free_block = freeList_;

		while (free_block != nullptr)
		{
			if (reinterpret_cast<uintptr_t>(free_block) >= block_end) {
				break;
			}

			prev_free_block = free_block;
			free_block = free_block->next;
		}

		if (prev_free_block == nullptr)
		{
			prev_free_block = reinterpret_cast<FreeBlock*>(block_start);
			prev_free_block->size = size;
			prev_free_block->next = freeList_;

			freeList_ = prev_free_block;
		}
		else if (reinterpret_cast<uintptr_t>(prev_free_block) + prev_free_block->size == block_start)
		{
			prev_free_block->size += size;
		}
		else
		{
			FreeBlock* temp = reinterpret_cast<FreeBlock*>(block_start);
			temp->size = size;
			temp->next = prev_free_block->next;
			prev_free_block->next = temp;

			prev_free_block = temp;
		}

		if (free_block != nullptr && reinterpret_cast<uintptr_t>(free_block) == block_end)
		{
			prev_free_block->size += free_block->size;
			prev_free_block->next = free_block->next;
		}

		numAllocs_--;
		usedMemory_ -= size;
	}

	PoolAllocator::PoolAllocator(void *start, size_t objectSize, size_t count, size_t alignment)
		: Allocator{start, ptrutil::alignSize(objectSize) * count}
		, objectSize_{ptrutil::alignSize(objectSize)}
		, alignment_{alignment} {

		size_t adjustment = ptrutil::alignForwardAdjustment(start_, alignment);

		freeList_ = static_cast<void**>(ptrutil::add(start_, adjustment));

		void** p = freeList_;

		for (size_t i = 0; i < count-1; i++) {
			*p = ptrutil::add(p, objectSize_);
			p = static_cast<void**>(*p);
		}

		*p = nullptr;
	}

	void* PoolAllocator::allocate() {
		if (freeList_ == nullptr) {
			return nullptr;
		}

		void *p = freeList_;

		freeList_ = static_cast<void**>(*freeList_);

		usedMemory_ += objectSize_;
		numAllocs_++;

		return p;
	}

	void PoolAllocator::deallocate(void *p) {
		*static_cast<void**>(p) = freeList_;
		freeList_ = static_cast<void**>(p);

		usedMemory_ -= objectSize_;
		numAllocs_--;
	}
}