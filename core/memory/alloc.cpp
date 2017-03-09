#include "alloc.h"

#include "memory_manager.h"

namespace oak {
	
	Allocator::Allocator(void *start, size_t size) : start_{start}, size_{size} {}

	LinearAllocator::LinearAllocator(void *start, size_t size) : Allocator{ start, size }, currentPos_{ start } {}

	void* LinearAllocator::allocate(size_t size, uint32_t alignment) {
		oak_assert(size != 0);

		uint32_t adjustment = ptrutil::alignForwardAdjustment(currentPos_, alignment);

		if (reinterpret_cast<uintptr_t>(currentPos_) - reinterpret_cast<uintptr_t>(start_) + adjustment + size > size_) {
			return nullptr;
		}

		uintptr_t alignedAddress = reinterpret_cast<uintptr_t>(currentPos_) + adjustment;

		currentPos_ = reinterpret_cast<void*>(alignedAddress + size);

		return reinterpret_cast<void*>(alignedAddress);
	}

	void LinearAllocator::deallocate(void *ptr, size_t size) {}

	void LinearAllocator::clear() {
		currentPos_ = start_;
	}



	StackAllocator::StackAllocator(void *start, size_t size) : Allocator{ start, size }, currentPos_{ start_ }, previousPos_{ nullptr } {}

	void* StackAllocator::allocate(size_t size, uint32_t alignment) {
		oak_assert(size != 0);

		previousPos_ = currentPos_;

		uint32_t adjustment = ptrutil::alignForwardAdjustmentWithHeader(currentPos_, alignment, sizeof(AllocationHeader));

		if (reinterpret_cast<uintptr_t>(currentPos_) - reinterpret_cast<uintptr_t>(start_) + adjustment + size > size_) {
			return nullptr;
		}

		void *alignedAddress = ptrutil::add(currentPos_, adjustment);

		AllocationHeader* header = static_cast<AllocationHeader*>(ptrutil::subtract(alignedAddress, sizeof(AllocationHeader)));

		header->adjustment = adjustment;
		header->prevAddress = previousPos_;

		currentPos_ = ptrutil::add(alignedAddress, size);

		return alignedAddress;
	}

	void StackAllocator::deallocate(void *p, size_t size) {
		if (p != previousPos_) { return; }

		AllocationHeader* header = static_cast<AllocationHeader*>(ptrutil::subtract(p, sizeof(AllocationHeader)));
		previousPos_ = header->prevAddress;
		currentPos_ = ptrutil::subtract(p, header->adjustment);

		if (currentPos_ == start_) {
			previousPos_ = nullptr;
		}
	}

	FreelistAllocator::FreelistAllocator(void *start, size_t size) : Allocator{ start, size }, freeList_{ static_cast<FreeBlock*>(start) } {
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
			header->size = total_size;
			header->adjustment = adjustment;

			oak_assert(ptrutil::alignForwardAdjustment((void*)aligned_address, alignment) == 0);

			return reinterpret_cast<void*>(aligned_address);
		}

		return nullptr;
	}

	void FreelistAllocator::deallocate(void *ptr, size_t size) {
		oak_assert(ptr != nullptr);

		AllocationHeader* header = static_cast<AllocationHeader*>(ptrutil::subtract(ptr, sizeof(AllocationHeader)));

		uintptr_t   block_start = reinterpret_cast<uintptr_t>(ptr) - header->adjustment;
		size_t block_size = header->size;
		uintptr_t   block_end = block_start + block_size;

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
			prev_free_block->size = block_size;
			prev_free_block->next = freeList_;

			freeList_ = prev_free_block;
		}
		else if (reinterpret_cast<uintptr_t>(prev_free_block) + prev_free_block->size == block_start)
		{
			prev_free_block->size += block_size;
		}
		else
		{
			FreeBlock* temp = reinterpret_cast<FreeBlock*>(block_start);
			temp->size = block_size;
			temp->next = prev_free_block->next;
			prev_free_block->next = temp;

			prev_free_block = temp;
		}

		if (free_block != nullptr && reinterpret_cast<uintptr_t>(free_block) == block_end)
		{
			prev_free_block->size += free_block->size;
			prev_free_block->next = free_block->next;
		}
	}

	void FreelistAllocator::append(void* memory, size_t size) {
		//find last free block
		FreeBlock *prevBlock = nullptr;
		FreeBlock *block = freeList_;

		while (block != nullptr) {
			prevBlock = block;
			block = block->next;
		}
		//create new free block with the added size
		FreeBlock *newBlock = static_cast<FreeBlock*>(memory);
		newBlock->size = size;
		newBlock->next = nullptr;
		
		if (prevBlock != nullptr) {
			prevBlock->next = newBlock;
		} else {
			freeList_->next = newBlock;
		}
	}

	PoolAllocator::PoolAllocator(void *start, size_t objectSize, size_t count, size_t alignment)
		: Allocator{ start, ptrutil::alignSize(objectSize) * count }
		, objectSize_{ ptrutil::alignSize(objectSize) }
		, alignment_{ alignment } {

		size_t adjustment = ptrutil::alignForwardAdjustment(start_, alignment);

		freeList_ = static_cast<void**>(ptrutil::add(start_, adjustment));

		void** p = freeList_;

		for (size_t i = 0; i < count-1; i++) {
			*p = ptrutil::add(p, objectSize_);
			p = static_cast<void**>(*p);
		}

		*p = nullptr;
	}

	void* PoolAllocator::allocate(size_t size, uint32_t alignment) {
		if (freeList_ == nullptr) {
			return nullptr;
		}

		void *p = freeList_;
		freeList_ = static_cast<void**>(*freeList_);

		return p;
	}

	void PoolAllocator::deallocate(void *p, size_t size) {
		*static_cast<void**>(p) = freeList_;
		freeList_ = static_cast<void**>(p);
	}

	namespace detail {

		void* allocate(Allocator *allocator, size_t size, uint32_t align) {
			std::cout << "allocating " << MemoryManager::memoryString(size) << std::endl;
			if (allocator == nullptr) {
				return MemoryManager::inst().allocate(size, align);
			}
			return allocator->allocate(size, align);
		}

		void deallocate(Allocator *allocator, void *ptr, size_t size) {
			std::cout << "deallocating " << MemoryManager::memoryString(size) << std::endl;
			if (allocator == nullptr) {
				MemoryManager::inst().deallocate(ptr, size);
			} else {
				allocator->deallocate(ptr, size);
			}
		}
	}
}