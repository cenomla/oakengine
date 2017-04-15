#pragma once

#include <utility>

#include "memory_manager.h"
#include "config.h"

namespace oak {

	template<class T>
	struct size_of_void {
		static constexpr size_t value = sizeof(T);
	};

	template<>
	struct size_of_void<void> {
		static constexpr size_t value = 1;
	};

	//base allocator
	template<class T, size_t A = config::DEFAULT_MEMORY_ALIGNMENT>
	class oak_allocator {
	public:
		template<class U, size_t L>
		friend class oak_allocator;

		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		
		static constexpr uint32_t align = A;
		static constexpr size_type value_size = size_of_void<T>::value;

		template<class U, size_t L = align>
		struct rebind {
			typedef oak_allocator<U, L> other;	
		};

		explicit oak_allocator(Allocator *allocator = &MemoryManager::inst().getGlobalAllocator(), size_t pageSize = config::GLOBAL_MEMORY_PAGE_SIZE) : allocator_{ allocator }, pageSize_{ pageSize } {}

		template<class U, size_t L = align>
		oak_allocator(const oak_allocator<U, L> &other) : allocator_{ other.allocator_ }, pageSize_{ other.pageSize_ } {}

		pointer allocate(size_t count, const_pointer locality = nullptr) {
			size_t size = count * value_size;
			void *ptr = allocator_->allocate(size, align);
			if (ptr == nullptr) {
				allocator_->append(MemoryManager::inst().allocate(pageSize_), pageSize_);
				ptr = allocator_->allocate(size, align);
			}
			return static_cast<pointer>(ptr);
		}

		void deallocate(void *ptr, size_t count) {
			size_t size = count * value_size;
			allocator_->deallocate(ptr, size);
		}

		size_type max_size() const {
			return allocator_ == nullptr ? config::GLOBAL_MEMORY_PAGE_SIZE / value_size : allocator_->getSize() / value_size;
		}

		template<class U, size_t L>
		inline bool equals(const oak_allocator<U, L> &second) const {
			return allocator_ == second.allocator_ && pageSize_ == second.pageSize_;
		}

	private:
		Allocator *allocator_;
		size_t pageSize_;
	};

	template<class T, size_t A, class U, size_t L>
	inline bool operator==(const oak_allocator<T, A> &first, const oak_allocator<U, L> &second) {
		return first.equals(second);
	}

	template<class T, size_t A, class U, size_t L>
	inline bool operator!=(const oak_allocator<T, A> &first, const oak_allocator<U, L> &second) {
		return !(first == second);
	}
	
	extern oak_allocator<void> proxyAllocator;
	extern oak_allocator<void> frameAllocator;

}