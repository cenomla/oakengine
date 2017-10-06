#pragma once

#include "types.h"

namespace oak {

	class Allocator {
	public:
		Allocator(Allocator *parent, uint32_t alignment);
		virtual ~Allocator();

		virtual void* allocate(size_t size) = 0;
		virtual void deallocate(void *ptr, size_t size) = 0;

		Allocator* getParent() { return parent_; }
		uint32_t getAlignment() const { return alignment_; }

	protected:
		Allocator *parent_;
		uint32_t alignment_;
	};


	class ProxyAllocator : public Allocator {
	public:
		ProxyAllocator();
		~ProxyAllocator();

		void* allocate(size_t size) override;
		void deallocate(void *ptr, size_t size) override;

	private:
		MemBlock *memList_;
		size_t numAllocs_;
	};

	class LinearAllocator : public Allocator {
	public:
		LinearAllocator(Allocator *parent, uint32_t alignment, size_t pageSize);
		~LinearAllocator();

		void* allocate(size_t size) override;
		void deallocate(void *ptr, size_t size) override;
		void clear();

		const void* getStart() const { return start_; }
	private:
		size_t pageSize_;
		void *start_;
		void *blockStart_;
		void *currentPos_;
		
		void grow();
	};

	class FreelistAllocator : public Allocator {
	public:
		FreelistAllocator(Allocator *parent, uint32_t alignment, size_t pageSize);
		~FreelistAllocator();

		void* allocate(size_t size) override;
		void deallocate(void *ptr, size_t size) override;
	private:
		struct AllocationHeader {
			size_t size;
			uint32_t adjustment;
		};

		size_t pageSize_;
		void *start_;
		MemBlock *freeList_;

		void grow(MemBlock *lastNode);
	};

	class PoolAllocator : public Allocator {
	public:
		PoolAllocator(Allocator *parent, uint32_t alignment, size_t pageSize, size_t objectSize);
		~PoolAllocator();

		void* allocate(size_t size) override;
		void deallocate(void *ptr, size_t size) override;
	private:
		size_t pageSize_;
		size_t objectSize_;
		void *start_;
		void **freeList_;

		void grow();
	};

}
