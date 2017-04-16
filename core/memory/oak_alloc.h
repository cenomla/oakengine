#pragma once

#include <utility>

#include "allocators.h"
#include "memory_literals.h"

namespace oak {

	extern ProxyAllocator oalloc_proxy;
	extern FreelistAllocator oalloc_freelist;
	extern LinearAllocator oalloc_frame;

	template<class T>
	struct size_of_void {
		static constexpr size_t value = sizeof(T);
	};

	template<>
	struct size_of_void<void> {
		static constexpr size_t value = 1;
	};

	//base allocator
	template<class T>
	class OakAllocator {
	public:
		template<class U>
		friend class OakAllocator;

		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		
		static constexpr size_type value_size = size_of_void<T>::value;

		template<class U>
		struct rebind {
			typedef OakAllocator<U> other;	
		};

		explicit OakAllocator(Allocator *allocator = &oalloc) : allocator_{ allocator } {}

		template<class U>
		OakAllocator(const OakAllocator<U> &other) : allocator_{ other.allocator_ } {}

		pointer allocate(size_t count, const_pointer locality = nullptr) {
			return static_cast<pointer>(allocator_->allocate(count * value_size));
		}

		void deallocate(void *ptr, size_t count) {
			allocator_->deallocate(ptr, count * value_size);
		}

		size_type max_size() const {
			return 8_gb;
		}

		template<class U>
		bool equals(const OakAllocator<U> &second) const {
			return allocator_ == second.allocator_;
		}

	private:
		Allocator *allocator_;
	};

	template<class T, class U>
	bool operator==(const OakAllocator<T> &first, const OakAllocator<U> &second) {
		return first.equals(second);
	}

	template<class T, class U>
	bool operator!=(const OakAllocator<T> &first, const OakAllocator<U> &second) {
		return !(first == second);
	}
	
	extern OakAllocator<void> oak_allocator;
	extern OakAllocator<void> frame_allocator;

}