#pragma once

#include <cstddef>
#include <cinttypes>

#include "memory_literals.h"

namespace oak {

	namespace detail {
		struct Block {
			void *next;
			size_t size;
		};
	}

	class Allocator {
	public:
		Allocator(Allocator *parent, uint32_t alignment);

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

		void* allocator(size_t size) override;
		void deallocate(void *ptr, size_t size) override;

	private:
		detail::Block *memList_;
		size_t numAllocs_;
	};

	class LinearAllocator : public Allocator {
	public:
		LinearAllocator(Allocator *parent, size_t pageSize = 32_kb, uint32_t alignment = 8);
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
		FreelistAllocator(Allocator *parent, size_t pageSize = 32_mb, uint32_t alignment = 8);
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
		detail::Block *freeList_;

		void grow(detail::Block *lastNode);
	};

	class PoolAllocator : public Allocator {
	public:
		PoolAllocator(Allocator *parent, size_t pageSize, size_t objectSize, uint32_t alignment = 8);
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