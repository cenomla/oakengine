#pragma once

#include <cstddef>
#include <cinttypes>

#include "config.h"

namespace oak {

	class Allocator {
	public:
		Allocator(void* start, size_t size);

		virtual void* allocate(size_t size, uint32_t alignment) = 0;
		virtual void deallocate(void *ptr, size_t size) = 0;
		virtual void append(void *ptr, size_t size) = 0;
		inline void* getStart() { return start_; }
		inline const void* getStart() const { return start_; }
		inline size_t getSize() const { return size_; }

	protected:
		void *start_;
		size_t size_;
	};

	class LinearAllocator : public Allocator {
	public:
		LinearAllocator();
		LinearAllocator(void *start, size_t size);
		
		void* allocate(size_t size, uint32_t alignment = config::DEFAULT_MEMORY_ALIGNMENT) override;
		void deallocate(void *ptr, size_t size) override;
		void append(void *ptr, size_t size) override;
		void clear();
	private:
		struct AllocationHeader {
			void *nextBlock;
			size_t size;
		};
		void *blockStart_;
		void *currentPos_;
	};

	class StackAllocator : public Allocator {
	public:
		StackAllocator(void *start, size_t size);

		void* allocate(size_t size, uint32_t alignment = config::DEFAULT_MEMORY_ALIGNMENT) override;
		void deallocate(void *p, size_t size) override;

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
		FreelistAllocator();
		FreelistAllocator(void *start, size_t size);

		void* allocate(size_t size, uint32_t alignment = config::DEFAULT_MEMORY_ALIGNMENT) override;
		void deallocate(void *ptr, size_t size) override;

		void append(void *ptr, size_t size) override;

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
		PoolAllocator();
		PoolAllocator(void *start, size_t size, size_t objectSize, uint32_t alignment = config::DEFAULT_MEMORY_ALIGNMENT);

		void* allocate(size_t size, uint32_t alignment) override;
		void deallocate(void *ptr, size_t size) override;

		void append(void *ptr, size_t size) override;

	private:
		void** freeList_;
		size_t objectSize_;
		size_t alignment_;
	};

}