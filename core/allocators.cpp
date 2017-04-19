#include "allocators.h"

#include "util/ptr_util.h"
#include "oak_assert.h"
#include "log.h"

namespace oak {

	Allocator::Allocator(Allocator *parent, uint32_t alignment) : parent_{ parent }, alignment_{ alignment } {}

	ProxyAllocator::ProxyAllocator() : Allocator{ nullptr, 64 }, memList_{ nullptr }, numAllocs_{ 0 } {

	}

	ProxyAllocator::~ProxyAllocator() {
		//free memory
		if (numAllocs_ > 0) {
			log_print_warn("memory leak, remaining blocks: %u", numAllocs_);
		}

		detail::Block *p = memList_;
		detail::Block *next = nullptr;
		while (p != nullptr) {
			next = static_cast<detail::Block*>(p->next);
			free(p);
			p = next;
			numAllocs_ --;
		}

	}

	void* ProxyAllocator::allocate(size_t size) {
		void *ptr = aligned_alloc(64, size + sizeof(detail::Block));
		detail::Block *l = static_cast<detail::Block*>(ptr);
		l->next = memList_;
		l->size = size;
		memList_ = l;

		numAllocs_ ++;

		return ptrutil::add(ptr, sizeof(detail::Block));
	}

	void ProxyAllocator::deallocate(void *ptr, size_t size) {
		//search through linked list for ptr - sizeof(memList) then remove from list and deallocate
		ptr = ptrutil::subtract(ptr, sizeof(detail::Block));
		
		detail::Block *p = memList_;
		detail::Block *prev = nullptr;
		while (p != nullptr) {
			if (p == ptr) {
				if (prev != nullptr) {
					prev->next = p->next;
				} else {
					memList_ = static_cast<detail::Block*>(p->next);
				}
				numAllocs_--;
				free(p);
				break;
			}
			prev = p;
			p = static_cast<detail::Block*>(p->next);
		}
	}

	LinearAllocator::LinearAllocator(Allocator *parent, size_t pageSize, uint32_t alignment) : Allocator{ parent, alignment }, pageSize_{ pageSize } {
		oak_assert(pageSize_ > sizeof(detail::Block));
		start_ = parent_->allocate(pageSize_ + sizeof(detail::Block));
		detail::Block *header = static_cast<detail::Block*>(start_);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(detail::Block);
		blockStart_ = start_;
		currentPos_ = ptrutil::add(start_, sizeof(detail::Block));
	}

	LinearAllocator::~LinearAllocator() {
		//deallocate used memory
		detail::Block *p = static_cast<detail::Block*>(start_);
		detail::Block *prev = nullptr;
		while (p) {
			prev = p;
			p = static_cast<detail::Block*>(p->next);

			parent_->deallocate(prev->next, prev->size);
		}
	}

	void* LinearAllocator::allocate(size_t size) {
		oak_assert(size != 0 && size <= pageSize_);

		uint32_t adjustment = ptrutil::alignForwardAdjustment(currentPos_, alignment_);
		uintptr_t alignedAddress = reinterpret_cast<uintptr_t>(currentPos_) + adjustment;
		detail::Block *header = static_cast<detail::Block*>(blockStart_);

		if (alignedAddress - reinterpret_cast<uintptr_t>(blockStart_) + size > header->size) {
			if (header->next == nullptr) {
				grow();
			}
			blockStart_ = header->next;
			currentPos_ = ptrutil::add(blockStart_, sizeof(detail::Block));
			adjustment = ptrutil::alignForwardAdjustment(currentPos_, alignment_);
			alignedAddress = reinterpret_cast<uintptr_t>(currentPos_) + adjustment;
		}

		currentPos_ = reinterpret_cast<void*>(alignedAddress + size);
		return reinterpret_cast<void*>(alignedAddress);
	}

	void LinearAllocator::deallocate(void *ptr, size_t size) {}

	void LinearAllocator::clear() {
		blockStart_ = start_;
		currentPos_ = ptrutil::add(blockStart_, sizeof(detail::Block));
	}

	void LinearAllocator::grow() {
		void *ptr = parent_->allocate(pageSize_ + sizeof(detail::Block));
		oak_assert(ptr != nullptr);
		detail::Block *header = static_cast<detail::Block*>(blockStart_);
		header->next = ptr;
		detail::Block *nHeader = static_cast<detail::Block*>(ptr);
		nHeader->next = nullptr;
		nHeader->size = pageSize_ + sizeof(detail::Block);
	}

	FreelistAllocator::FreelistAllocator(Allocator *parent, size_t pageSize, uint32_t alignment) : 
	Allocator{ parent, alignment }, pageSize_{ pageSize } {
		oak_assert(pageSize_ > sizeof(detail::Block));
		start_ = parent_->allocate(pageSize_ + sizeof(detail::Block));
		detail::Block *header = static_cast<detail::Block*>(start_);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(detail::Block);

		freeList_ = header + 1;
		freeList_->size = pageSize_;
		freeList_->next = nullptr;
	}

	FreelistAllocator::~FreelistAllocator() {
		//deallocate used memory
		detail::Block *p = static_cast<detail::Block*>(start_);
		detail::Block *prev = nullptr;
		while (p) {
			prev = p;
			p = static_cast<detail::Block*>(p->next);

			parent_->deallocate(prev->next, prev->size);
		}
	}

	void* FreelistAllocator::allocate(size_t size) {
		oak_assert(size != 0 && size <= pageSize_);
		detail::Block *prev = nullptr;
		detail::Block *p = freeList_;

		while (p) {
			//Calculate adjustment needed to keep object correctly aligned
			uint32_t adjustment = ptrutil::alignForwardAdjustmentWithHeader(p, alignment_, sizeof(AllocationHeader));

			size_t totalSize = size + adjustment;

			//If allocation doesn't fit in this FreeBlock, try the next
			if (p->size < totalSize) {
				prev = p;
				p = static_cast<detail::Block*>(p->next);
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
					freeList_ = static_cast<detail::Block*>(p->next);
				}
			} else {
				//else create a new FreeBlock containing remaining memory
				detail::Block* next = static_cast<detail::Block*>(ptrutil::add(p, totalSize));
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

			oak_assert(ptrutil::alignForwardAdjustment(reinterpret_cast<void*>(aligned_address), alignment_) == 0);

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

		detail::Block *prev = nullptr;
		detail::Block *p = freeList_;

		while (p) {
			if (reinterpret_cast<uintptr_t>(p) >= blockEnd) {
				break;
			}

			prev = p;
			p = static_cast<detail::Block*>(p->next);
		}

		if (prev == nullptr) {
			prev = reinterpret_cast<detail::Block*>(blockStart);
			prev->size = blockSize;
			prev->next = freeList_;

			freeList_ = prev;
		} else if (reinterpret_cast<uintptr_t>(prev) + prev->size == blockStart) {
			prev->size += blockSize;
		} else {
			detail::Block *temp = reinterpret_cast<detail::Block*>(blockStart);
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

	void FreelistAllocator::grow(detail::Block *lastNode) {
		
		//find the end of the used block list
		detail::Block *prevHeader = nullptr;
		detail::Block *header = static_cast<detail::Block*>(start_);

		while (header) {
			prevHeader = header;
			header = static_cast<detail::Block*>(header->next);
		}

		//after loop prev = end of free list

		//create new memory block and append it to the used block list
		void *ptr = parent_->allocate(pageSize_ + sizeof(detail::Block));
		oak_assert(ptr != nullptr);
		prevHeader->next = ptr;
		header = static_cast<detail::Block*>(ptr);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(detail::Block);

		//create new free block with the added size
		detail::Block *newBlock = header + 1;
		newBlock->size = pageSize_;
		newBlock->next = nullptr;
		
		lastNode->next = newBlock;
	}

	PoolAllocator::PoolAllocator(Allocator *parent, size_t pageSize, size_t objectSize, uint32_t alignment) : 
	Allocator{ parent, alignment }, pageSize_{ pageSize }, objectSize_{ ptrutil::alignSize(objectSize, alignment_) } {
		start_ = parent_->allocate(pageSize_ + sizeof(detail::Block));
		detail::Block *header = static_cast<detail::Block*>(start_);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(detail::Block);

		size_t count = (pageSize_ & ~(alignment_-1)) / objectSize_;

		freeList_ = static_cast<void**>(ptrutil::add(start_, sizeof(detail::Block)));

		void **p = freeList_;

		for (size_t i = 0; i < count - 1; i++) {
			*p = ptrutil::add(p, objectSize_);
			p = static_cast<void**>(*p);
		}

		*p = nullptr;

	}

	PoolAllocator::~PoolAllocator() {
		//deallocate used memory
		detail::Block *p = static_cast<detail::Block*>(start_);
		detail::Block *prev = nullptr;
		while (p) {
			prev = p;
			p = static_cast<detail::Block*>(p->next);

			parent_->deallocate(prev->next, prev->size);
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
		detail::Block *prevHeader = nullptr;
		detail::Block *header = static_cast<detail::Block*>(start_);

		while (header) {
			prevHeader = header;
			header = static_cast<detail::Block*>(header->next);
		}

		//after loop prev = end of free list

		//create new memory block and append it to the used block list
		void *ptr = parent_->allocate(pageSize_ + sizeof(detail::Block));
		oak_assert(ptr != nullptr);
		prevHeader->next = ptr;
		header = static_cast<detail::Block*>(ptr);
		header->next = nullptr;
		header->size = pageSize_ + sizeof(detail::Block);

		size_t count = (pageSize_ & ~(alignment_-1)) / objectSize_;

		*prev = ptrutil::add(ptr, sizeof(detail::Block));

		p = static_cast<void**>(*prev);

		for (size_t i = 0; i < count - 1; i++) {
			*p = ptrutil::add(p, objectSize_);
			p = static_cast<void**>(*p);
		}

		*p = nullptr;
	}

}