#pragma once

#include <utility>
#include <cstddef>
#include <cinttypes>

namespace oak {

	namespace detail {
		void* allocate(size_t size, uint32_t pool, uint32_t align);
		void deallocate(void *ptr, size_t size, uint32_t pool);
		size_t blockSize(uint32_t pool);
	}

	template<class T, size_t P = 0, size_t A = 8>
	class OakAllocator {
	public:
		static constexpr uint32_t pool = P;
		static constexpr uint32_t align = A;

		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		template<class U, size_t N = 0, size_t L = 8>
		struct rebind {
			typedef OakAllocator<U, N, L> other;	
		};

		OakAllocator() = default;

		template<class U, size_t N = 0, size_t L = 8>
		OakAllocator(const OakAllocator<U, N, L> &other) {}

		pointer allocate(size_t size, const_pointer locality = nullptr) {
			return static_cast<pointer>(detail::allocate(size * sizeof(value_type), pool, align));
		}

		void deallocate(void *ptr, size_t size) {
			detail::deallocate(ptr, size * sizeof(value_type), pool);
		}

		template<class... TArgs>
		inline void construct(pointer ptr, TArgs&&... args) {
			new (ptr) value_type{ std::forward<TArgs>(args)... };
		}

		inline void destroy(pointer ptr) {
			ptr->~value_type();
		}

		size_type max_size() const {
			return detail::blockSize(pool) / sizeof(value_type);
		}
	};

	template<class T, size_t P, size_t A, class U, size_t N, size_t L>
	inline bool operator==(const OakAllocator<T, P, A> &first, const OakAllocator<U, N, L> &second) {
		return first.align == second.align && first.pool == second.pool;
	}

	template<class T, size_t P, size_t A, class U, size_t N, size_t L>
	inline bool operator!=(const OakAllocator<T, P, A> &first, const OakAllocator<U, N, L> &second) {
		return !(first == second);
	}

}