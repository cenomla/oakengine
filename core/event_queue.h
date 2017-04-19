#pragma once

#include "util/ptr_util.h"
#include "container.h"

namespace oak {

	class EventQueueBase {
	public:
		EventQueueBase(size_t eventSize) : 
		allocator_{ &oalloc_freelist, 1024 * eventSize, 8 },
		size_{ 0 } {}

		void clear() {
			allocator_.clear();
			size_ = 0;
		}

	protected:
		LinearAllocator allocator_;
		size_t size_;
	};

	template <class TEvent>
	class EventQueue : public EventQueueBase {
	public:
		static constexpr size_t EVENT_SIZE = ptrutil::alignSize(detail::size_of_void<TEvent>::value, 8);

		class const_iterator : public std::iterator<std::forward_iterator_tag, TEvent> {
		public:

			explicit const_iterator(const detail::Block *header, size_t count = 0) : 
			header_{ header }, 
			ptr_{ static_cast<const TEvent*>(ptrutil::add(header_, sizeof(detail::Block))) },
			count_{ count } {}

			const_iterator& operator++() {
				ptr_ = static_cast<const TEvent*>(ptrutil::add(ptr_, EVENT_SIZE));
				count_++;
				if (ptrutil::add(ptr_, EVENT_SIZE) > ptrutil::add(header_, header_->size)) {
					header_ = static_cast<const detail::Block*>(header_->next);
					ptr_ = static_cast<const TEvent*>(ptrutil::add(header_, sizeof(detail::Block)));
				}

				return *this;
			}

			bool operator==(const const_iterator& other) const { return count_ == other.count_; }
			bool operator!=(const const_iterator& other) const { return !(*this == other); }
			const TEvent& operator*() const { return *ptr_; }

		private:
			const detail::Block *header_;
			const TEvent *ptr_;
			size_t count_;
		};

		EventQueue() : EventQueueBase{ EVENT_SIZE } {}

		const_iterator begin() const { return const_iterator{ static_cast<const detail::Block*>(allocator_.getStart()) }; }
		const_iterator end() const { return const_iterator{ nullptr, size_ }; }

		void emit(const TEvent& event) {
			void *ptr = allocator_.allocate(EVENT_SIZE);
			new (ptr) TEvent{ event };
			size_++;
		}
	};

}
