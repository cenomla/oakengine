#pragma once

#include "util/pointer_util.h"
#include "oak_assert.h"
#include "block.h"

namespace oak {

	class Allocator {
	public:
		Allocator(void* start, size_t size);

		inline void* getStart() { return start_; }
		inline const void* getStart() const { return start_; }
		inline size_t getSize() const { return size_; }
		inline size_t getUsedMemory() const { return usedMemory_; }
		inline size_t getNumAllocs() const { return numAllocs_; }

	protected:
		void *start_;
		size_t size_;

		size_t usedMemory_;
		size_t numAllocs_;
	};

	class LinearAllocator : public Allocator {
	public:
		LinearAllocator(void *start, size_t size);
		
		void* allocate(size_t size, uint32_t alignment = 8);
		void clear();
	private:
		void *currentPos_;
	};

	class StackAllocator : public Allocator {
	public:
		StackAllocator(void *start, size_t size);

		void* allocate(size_t size, uint32_t alignment = 8);
		void deallocate(void *p);

		inline void* getTop() { return previousPos_; }
	private:
		struct AllocationHeader {
			void *prevAddress;
			size_t adjustment;
		};
		void *currentPos_;
		void *previousPos_;
	};

	class FreelistAllocator : public Allocator {
	public:
		FreelistAllocator(void *start, size_t size);

		void* allocate(size_t size, uint32_t alignment = 8);
		void deallocate(void *p, size_t size);

	private:
		struct AllocationHeader {
			size_t size;
			uint32_t adjustment;
		};

		struct FreeBlock {
			size_t size;
			FreeBlock *next;
		};
		FreeBlock* freeList_;
	};

	class PoolAllocator : public Allocator {
	public:
		PoolAllocator(void *start, size_t objectSize, size_t count, size_t alignment);

		void* allocate();
		void deallocate(void *p);

	private:
		void** freeList_;
		size_t objectSize_;
		size_t alignment_;
	};
	
}