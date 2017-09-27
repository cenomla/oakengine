#pragma once

#include <iterator>

#include "util/ptr_util.h"
#include "type_info.h"
#include "allocators.h"

namespace oak {

	struct EventQueueBase {
		EventQueueBase(const TypeInfo *tinfo); 

		void clear();
		bool empty();
		void* next();

		size_t alignedSize, size;
		LinearAllocator allocator;
	};

	template<class T>
	struct EventQueue: public EventQueueBase {

		EventQueue() : EventQueueBase(&T::typeInfo) {}

		template<class... TArgs>
		void emit(TArgs&&... args) {
			auto ptr = next();
			new (ptr) T{ std::forward<TArgs>(args)... };
		}

		class const_iterator : public std::iterator<std::forward_iterator_tag, T> {
		public:
			static constexpr size_t ALIGNED_SIZE = ptrutil::alignSize(detail::size_of_void<T>::value, 16);

			explicit const_iterator(const detail::Block *header, size_t count = 0) :
			header_{ header },
			ptr_{ static_cast<const T*>(ptrutil::add(header_, sizeof(detail::Block))) },
			count_{ count - 1 },
			left_{ 256 } {}

			const_iterator& operator++() {
				count_--;
				left_--;
				ptr_ = static_cast<const T*>(ptrutil::add(ptr_, ALIGNED_SIZE));
				if (left_ > 0) {
					return *this;
				} else {
					header_ = static_cast<const detail::Block*>(header_->next);
					ptr_ = static_cast<const T*>(ptrutil::add(header_, sizeof(detail::Block)));
					left_ = 256;
					return *this;
				}
			}

			bool operator==(const const_iterator& other) const { return count_ == other.count_; }
			bool operator!=(const const_iterator& other) const { return !operator==(other); }

			const T& operator*() const { return *ptr_; }
		private:
			const detail::Block *header_;
			const T *ptr_;
			size_t count_, left_;
		};


		const_iterator begin() { return const_iterator{ static_cast<const detail::Block*>(allocator.getStart()), size }; }
		const_iterator end() { return const_iterator{ nullptr, 0 }; }

	};

}
