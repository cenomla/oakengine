#pragma once

#include "memory_manager.h"

namespace oak {

	template<typename T>
	class OakAllocator {
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		template<typename U>
		struct rebind {
			typedef MemAllocator<U> other;	
		};

		OakAllocator(uint32_t p = 0, uint32_t a = 8) : pool{ p }, alignment{ a } {}

		template<typename U>
		OakAllocator(const MemAllocator<U> &other) : pool{ other.pool }, alignment{ other.alignment } {}

		inline pointer allocate(size_t size, const_pointer locality = nullptr) {
			std::cout << "allocating " << MemoryManager::memoryString(size * sizeof(value_type)) << std::endl;
			return static_cast<pointer>(MemoryManager::inst().allocate(size * sizeof(value_type), pool, alignment));
		}

		inline void deallocate(void *ptr, size_t size) {

			std::cout << "deallocating " << MemoryManager::memoryString(size * sizeof(value_type)) << std::endl;
			MemoryManager::inst().deallocate(ptr, size * sizeof(value_type), pool);
		}

		template<typename... TArgs>
		inline void construct(pointer ptr, TArgs&&... args) {
			new (ptr) value_type{ std::forward<TArgs>(args)... };
		}

		inline void destroy(pointer ptr) {
			ptr->~value_type();
		}

		inline size_type max_size() const { return MemoryManager::inst().getPoolBlockSize(pool) / sizeof(value_type); }

		const uint32_t alignment;
		const uint32_t pool;
	};

	template<typename T, typename U>
	inline bool operator==(const MemAllocator<T> &first, const MemAllocator<U> &second) {
		return first.alignment == second.alignment && first.pool == second.pool;
	}

	template<typename T, typename U>
	inline bool operator!=(const MemAllocator<T> &first, const MemAllocator<U> &second) {
		return !(first == second);
	}

}