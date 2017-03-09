#pragma once

#include <utility>
#include <cstddef>
#include <cinttypes>

#include "util/pointer_util.h"
#include "memory_literals.h"
#include "oak_assert.h"
#include "block.h"

namespace oak {

	class Allocator {
	public:
		Allocator(void* start, size_t size);

		virtual void* allocate(size_t size, uint32_t alignment) = 0;
		virtual void deallocate(void *ptr, size_t size) = 0;
		inline void* getStart() { return start_; }
		inline const void* getStart() const { return start_; }
		inline size_t getSize() const { return size_; }

	protected:
		void *start_;
		size_t size_;
	};

	class LinearAllocator : public Allocator {
	public:
		LinearAllocator(void *start, size_t size);
		
		void* allocate(size_t size, uint32_t alignment = 8) override;
		void deallocate(void *ptr, size_t size) override;
		void clear();
	private:
		void *currentPos_;
	};

	class StackAllocator : public Allocator {
	public:
		StackAllocator(void *start, size_t size);

		void* allocate(size_t size, uint32_t alignment = 8) override;
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
		FreelistAllocator(void *start, size_t size);

		void* allocate(size_t size, uint32_t alignment = 8);
		void deallocate(void *ptr, size_t size);

		void append(void *ptr, size_t size);

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
		PoolAllocator(void *start, size_t objectSize, size_t count, size_t alignment = 8);

		void* allocate(size_t size, uint32_t alignment) override;
		void deallocate(void *ptr, size_t size) override;

	private:
		void** freeList_;
		size_t objectSize_;
		size_t alignment_;
	};

	namespace detail {
		void* allocate(Allocator *allocator, size_t size, uint32_t align);
		void deallocate(Allocator *allocator, void *ptr, size_t size);
	}

	template<class T, size_t A = 8>
	class OakAllocator {
	public:
		static constexpr uint32_t align = A;

		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		template<class U, size_t L = 8>
		struct rebind {
			typedef OakAllocator<U, L> other;	
		};

		OakAllocator(Allocator *allocator = nullptr) : allocator_{ allocator } {}

		template<class U, size_t L = 8>
		OakAllocator(const OakAllocator<U, L> &other) {}

		pointer allocate(size_t size, const_pointer locality = nullptr) {
			return static_cast<pointer>(detail::allocate(allocator_, size * sizeof(value_type), align));
		}

		void deallocate(void *ptr, size_t size) {
			detail::deallocate(allocator_, ptr, size * sizeof(value_type));
		}

		template<class... TArgs>
		inline void construct(pointer ptr, TArgs&&... args) {
			new (ptr) value_type{ std::forward<TArgs>(args)... };
		}

		inline void destroy(pointer ptr) {
			ptr->~value_type();
		}

		size_type max_size() const {
			return 4_mb / sizeof(value_type);
		}

		template<class U, size_t L>
		inline bool equals(const OakAllocator<U, L> &second) const {
			return allocator_ == second.allocator_;
		}

	private:
		Allocator *allocator_ = nullptr;
	};

	template<class T, size_t A, class U, size_t L>
	inline bool operator==(const OakAllocator<T, A> &first, const OakAllocator<U, L> &second) {
		return first.equals(second);
	}

	template<class T, size_t A, class U, size_t L>
	inline bool operator!=(const OakAllocator<T, A> &first, const OakAllocator<U, L> &second) {
		return !(first == second);
	}
	
}