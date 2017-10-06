#include "allocator.h"

#include "ptr_util.h"
#include "oak_assert.h"
#include "log.h"

namespace oak {

	Allocator::Allocator(Allocator *parent, uint32_t alignment) : parent_{ parent }, alignment_{ alignment } {}

	Allocator::~Allocator() {}

	ProxyAllocator::ProxyAllocator() : Allocator{ nullptr, 64 }, memList_{ nullptr }, numAllocs_{ 0 } {

	}

	ProxyAllocator::~ProxyAllocator() {
		//free memory
		if (numAllocs_ > 0) {
			log_print_warn("memory leak, remaining blocks: %u", numAllocs_);
		}

		MemBlock *p = memList_;
		MemBlock *next = nullptr;
		while (p != nullptr) {
			next = static_cast<MemBlock*>(p->next);
			free(p);
			p = next;
			numAllocs_ --;
		}

	}

	namespace debug::vars {
		extern size_t allocatedMemory;
	}
	void* ProxyAllocator::allocate(size_t size) {
		void *ptr = aligned_alloc(64, size + sizeof(MemBlock));
		MemBlock *l = static_cast<MemBlock*>(ptr);
		l->next = memList_;
		l->size = size;
		memList_ = l;

		numAllocs_ ++;

		debug::vars::allocatedMemory += size + sizeof(MemBlock);

		return ptrutil::add(ptr, sizeof(MemBlock));
	}

	void ProxyAllocator::deallocate(void *ptr, size_t size) {
		//search through linked list for ptr - sizeof(memList) then remove from list and deallocate
		ptr = ptrutil::subtract(ptr, sizeof(MemBlock));

		debug::vars::allocatedMemory -= size + sizeof(MemBlock);
		
		MemBlock *p = memList_;
		MemBlock *prev = nullptr;
		while (p != nullptr) {
			if (p == ptr) {
				if (prev != nullptr) {
					prev->next = p->next;
				} else {
					memList_ = static_cast<MemBlock*>(p->next);
				}
				numAllocs_--;
				free(p);
				break;
			}
			prev = p;
			p = static_cast<MemBlock*>(p->next);
		}
	}

	LinearAllocator::LinearAllocator(Allocator *parent, uint32_t alignment, size_t pageSize) : Allocator{ parent, alignment }, pageSize_{ pageSize } {
		oak_assert(pageSize_ > sizeof(MemBlock));
		oak_assert(parent->getAlignment() >= alignment);
		start_ = parent_->allocate(pageSize_ + sizeof(MemBlock));
		MemBlock *header = static_cast<MemBlock*>(start_);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(MemBlock);
		blockStart_ = start_;
		currentPos_ = ptrutil::add(start_, sizeof(MemBlock));
	}

	LinearAllocator::~LinearAllocator() {
		//deallocate used memory
		MemBlock *prev = nullptr;
		MemBlock *p = static_cast<MemBlock*>(start_);
		while (p) {
			prev = p;
			p = static_cast<MemBlock*>(p->next);

			parent_->deallocate(prev, prev->size);
		}
	}

	void* LinearAllocator::allocate(size_t size) {
		oak_assert(size != 0 && size <= pageSize_);

		uint32_t adjustment = ptrutil::align_offset(currentPos_, alignment_);
		uintptr_t alignedAddress = reinterpret_cast<uintptr_t>(currentPos_) + adjustment;
		MemBlock *header = static_cast<MemBlock*>(blockStart_);

		if (alignedAddress - reinterpret_cast<uintptr_t>(blockStart_) + size > header->size) {
			if (header->next == nullptr) {
				grow();
			}
			blockStart_ = header->next;
			currentPos_ = ptrutil::add(blockStart_, sizeof(MemBlock));
			adjustment = ptrutil::align_offset(currentPos_, alignment_);
			alignedAddress = reinterpret_cast<uintptr_t>(currentPos_) + adjustment;
		}

		currentPos_ = reinterpret_cast<void*>(alignedAddress + size);
		return reinterpret_cast<void*>(alignedAddress);
	}

	void LinearAllocator::deallocate(void *ptr, size_t size) {}

	void LinearAllocator::clear() {
		blockStart_ = start_;
		currentPos_ = ptrutil::add(blockStart_, sizeof(MemBlock));
	}

	void LinearAllocator::grow() {
		void *ptr = parent_->allocate(pageSize_ + sizeof(MemBlock));
		oak_assert(ptr != nullptr);
		MemBlock *header = static_cast<MemBlock*>(blockStart_);
		header->next = ptr;
		MemBlock *nHeader = static_cast<MemBlock*>(ptr);
		nHeader->next = nullptr;
		nHeader->size = pageSize_ + sizeof(MemBlock);
	}

	FreelistAllocator::FreelistAllocator(Allocator *parent, uint32_t alignment, size_t pageSize) : 
	Allocator{ parent, alignment }, pageSize_{ pageSize } {
		oak_assert(pageSize_ > sizeof(MemBlock));
		start_ = parent_->allocate(pageSize_ + sizeof(MemBlock));
		MemBlock *header = static_cast<MemBlock*>(start_);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(MemBlock);

		freeList_ = header + 1;
		freeList_->size = pageSize_;
		freeList_->next = nullptr;
	}

	FreelistAllocator::~FreelistAllocator() {
		//deallocate used memory
		MemBlock *prev = nullptr;
		MemBlock *p = static_cast<MemBlock*>(start_);
		while (p) {
			prev = p;
			p = static_cast<MemBlock*>(p->next);

			parent_->deallocate(prev, prev->size);
		}
	}

	void* FreelistAllocator::allocate(size_t size) {
		oak_assert(size != 0 && size <= pageSize_);
		MemBlock *prev = nullptr;
		MemBlock *p = freeList_;

		while (p) {
			//Calculate adjustment needed to keep object correctly aligned
			uint32_t adjustment = ptrutil::align_offset_width_header(p, alignment_, sizeof(AllocationHeader));

			size_t totalSize = size + adjustment;

			//If allocation doesn't fit in this FreeBlock, try the next
			if (p->size < totalSize) {
				prev = p;
				p = static_cast<MemBlock*>(p->next);
				if (p->next == nullptr) {
					grow(p);
				}
				continue;
			}

			//If allocations in the remaining memory will be impossible
			if (p->size - totalSize <= sizeof(AllocationHeader)) {
				//Increase allocation size instead of creating a new FreeBlock
				totalSize = p->size;

				if (prev != nullptr) {
					prev->next = p->next;
				} else {
					freeList_ = static_cast<MemBlock*>(p->next);
				}
			} else {
				//else create a new FreeBlock containing remaining memory
				MemBlock* next = static_cast<MemBlock*>(ptrutil::add(p, totalSize));
				next->size = p->size - totalSize;
				next->next = p->next;

				if (prev != nullptr) {
					prev->next = next;
				}
				else {
					freeList_ = next;
				}
			}

			uintptr_t aligned_address = reinterpret_cast<uintptr_t>(p) + adjustment;

			AllocationHeader* header = reinterpret_cast<AllocationHeader*>(aligned_address - sizeof(AllocationHeader));
			header->size = totalSize;
			header->adjustment = adjustment;

			oak_assert(ptrutil::align_offset(reinterpret_cast<void*>(aligned_address), alignment_) == 0);

			return reinterpret_cast<void*>(aligned_address);
		}

		return nullptr;
	}

	void FreelistAllocator::deallocate(void *ptr, size_t size) {
		oak_assert(ptr != nullptr);

		AllocationHeader* header = static_cast<AllocationHeader*>(ptrutil::subtract(ptr, sizeof(AllocationHeader)));

		uintptr_t   blockStart = reinterpret_cast<uintptr_t>(ptr) - header->adjustment;
		size_t blockSize = header->size;
		uintptr_t   blockEnd = blockStart + blockSize;

		MemBlock *prev = nullptr;
		MemBlock *p = freeList_;

		while (p) {
			if (reinterpret_cast<uintptr_t>(p) >= blockEnd) {
				break;
			}

			prev = p;
			p = static_cast<MemBlock*>(p->next);
		}

		if (prev == nullptr) {
			prev = reinterpret_cast<MemBlock*>(blockStart);
			prev->size = blockSize;
			prev->next = freeList_;

			freeList_ = prev;
		} else if (reinterpret_cast<uintptr_t>(prev) + prev->size == blockStart) {
			prev->size += blockSize;
		} else {
			MemBlock *temp = reinterpret_cast<MemBlock*>(blockStart);
			temp->size = blockSize;
			temp->next = prev->next;
			prev->next = temp;

			prev = temp;
		}

		if (p != nullptr && reinterpret_cast<uintptr_t>(p) == blockEnd) {
			prev->size += p->size;
			prev->next = p->next;
		}
	}

	void FreelistAllocator::grow(MemBlock *lastNode) {
		
		//find the end of the used block list
		MemBlock *prevHeader = nullptr;
		MemBlock *header = static_cast<MemBlock*>(start_);

		while (header) {
			prevHeader = header;
			header = static_cast<MemBlock*>(header->next);
		}

		//after loop prev = end of free list

		//create new memory block and append it to the used block list
		void *ptr = parent_->allocate(pageSize_ + sizeof(MemBlock));
		oak_assert(ptr != nullptr);
		prevHeader->next = ptr;
		header = static_cast<MemBlock*>(ptr);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(MemBlock);

		//create new free block with the added size
		MemBlock *newBlock = header + 1;
		newBlock->size = pageSize_;
		newBlock->next = nullptr;
		
		lastNode->next = newBlock;
	}

	PoolAllocator::PoolAllocator(Allocator *parent, uint32_t alignment, size_t pageSize, size_t objectSize) : 
	Allocator{ parent, alignment }, pageSize_{ pageSize }, objectSize_{ ptrutil::align_size(objectSize, alignment_) } {
		start_ = parent_->allocate(pageSize_ + sizeof(MemBlock));
		MemBlock *header = static_cast<MemBlock*>(start_);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(MemBlock);

		size_t count = (pageSize_ & ~(alignment_-1)) / objectSize_;

		freeList_ = static_cast<void**>(ptrutil::add(start_, sizeof(MemBlock)));

		void **p = freeList_;

		for (size_t i = 0; i < count - 1; i++) {
			*p = ptrutil::add(p, objectSize_);
			p = static_cast<void**>(*p);
		}

		*p = nullptr;

	}

	PoolAllocator::~PoolAllocator() {
		//deallocate used memory
		MemBlock *prev = nullptr;
		MemBlock *p = static_cast<MemBlock*>(start_);
		while (p) {
			prev = p;
			p = static_cast<MemBlock*>(p->next);

			parent_->deallocate(prev, prev->size);
		}
	}

	void* PoolAllocator::allocate(size_t size) {
		if (*freeList_ == nullptr) {
			grow();
		}

		void *p = freeList_;
		freeList_ = static_cast<void**>(*freeList_);

		return p;
	}

	void PoolAllocator::deallocate(void *p, size_t size) {
		*static_cast<void**>(p) = freeList_;
		freeList_ = static_cast<void**>(p);
	}

	void PoolAllocator::grow() {
		//find end of freelist
		void **prev = nullptr;
		void **p = freeList_;

		while (p) {
			prev = p;
			p = static_cast<void**>(*p);
		}

		//find the end of the used block list
		MemBlock *prevHeader = nullptr;
		MemBlock *header = static_cast<MemBlock*>(start_);

		while (header) {
			prevHeader = header;
			header = static_cast<MemBlock*>(header->next);
		}

		//after loop prev = end of free list

		//create new memory block and append it to the used block list
		void *ptr = parent_->allocate(pageSize_ + sizeof(MemBlock));
		oak_assert(ptr != nullptr);
		prevHeader->next = ptr;
		header = static_cast<MemBlock*>(ptr);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(MemBlock);

		size_t count = (pageSize_ & ~(alignment_-1)) / objectSize_;

		*prev = ptrutil::add(ptr, sizeof(MemBlock));

		p = static_cast<void**>(*prev);

		for (size_t i = 0; i < count - 1; i++) {
			*p = ptrutil::add(p, objectSize_);
			p = static_cast<void**>(*p);
		}

		*p = nullptr;
	}

}
