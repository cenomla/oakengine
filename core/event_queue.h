#pragma once

#include <iterator>

#include "util/ptr_util.h"
#include "type_info.h"
#include "allocators.h"

namespace oak {

	struct EventQueue {
		EventQueue(const TypeInfo *tinfo); 

		void clear();
		void empty();
		void* emit();

		size_t alignedSize, size;
		LinearAllocator allocator;
	};

	template<class T>
	struct EventQueueIter : public EventQueue {
		using EventQueue::EventQueue;

		class EventIterator : public std::iterator<std::forward_iterator_tag, T> {
		public:
			static constexpr size_t ALIGNED_SIZE = ptrutil::alignSize(detail::size_of_void<T>::value, 16);

			explicit EventIterator(const detail::Block *header, size_t count = 0) :
			header_{ header },
			ptr_{ static_cast<const T*>(ptrutil::add(header_, sizeof(detail::Block))) },
			count_{ count }, left_{ (header->size - sizeof(detail::Block) / ALIGNED_SIZE } {}

			EventIterator& operator++() {
				ptr_ = static_cast<const T*>(ptrutil::add(ptr_, ALIGNED_SIZE));
				count_--;
				left_--;
				if (left_ > 0) {
					return *this;
				} else {
					header_ = static_cast<const detail::Block*>(header_->next);
					ptr_ = static_cast<const T*>(ptrutil::add(header_, sizeof(detail::Block)));
					left_ = (header_->size - sizeof(detail::Block)) / ALIGNED_SIZE;
					return *this;
				}
			}

			bool operator==(const EventIterator& other) const { return count_ == other.count_; }
			bool operator!=(const EventIterator& other) const { return !operator==(other); }

			const T& operator*() const { return *ptr_; }
		private:
			const detail::Block *header_;
			const T *ptr_;
			size_t count_, left_;
		};


		EventIterator begin() { return EventIterator{ static_cast<detail::Block*>(allocator.getStart()), size }; }
		EventIterator end() { return EventIterator{ nullptr, 0 }; }
	};

}
