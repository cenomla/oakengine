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
	class OakAllocator {
	public:
		template<class U, size_t L>
		friend class OakAllocator;

		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		
		static constexpr uint32_t align = A;
		static constexpr size_type value_size = size_of_void<T>::value;

		template<class U, size_t L = align>
		struct rebind {
			typedef OakAllocator<U, L> other;	
		};

		OakAllocator(Allocator *allocator = &MemoryManager::inst().getGlobalAllocator(), size_t pageSize = config::GLOBAL_MEMORY_PAGE_SIZE) : allocator_{ allocator }, pageSize_{ pageSize } {}

		template<class U, size_t L = align>
		OakAllocator(const OakAllocator<U, L> &other) : allocator_{ other.allocator_ }, pageSize_{ other.pageSize_ } {}

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
		inline bool equals(const OakAllocator<U, L> &second) const {
			return allocator_ == second.allocator_ && pageSize_ == second.pageSize_;
		}

	private:
		Allocator *allocator_;
		size_t pageSize_;
	};

	template<class T, size_t A, class U, size_t L>
	inline bool operator==(const OakAllocator<T, A> &first, const OakAllocator<U, L> &second) {
		return first.equals(second);
	}

	template<class T, size_t A, class U, size_t L>
	inline bool operator!=(const OakAllocator<T, A> &first, const OakAllocator<U, L> &second) {
		return !(first == second);
	}
	
	extern OakAllocator<void> proxyAllocator;
	extern OakAllocator<void> frameAllocator;

}