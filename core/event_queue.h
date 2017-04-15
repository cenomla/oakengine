#pragma once

#include "memory/oak_alloc.h"
#include "util/ptr_util.h"
#include "container.h"

namespace oak {

	template <class TEvent>
	class EventQueue {
	public:
		static constexpr size_t EVENT_SIZE = ptrutil::alignSize(sizeof(TEvent), 8);
		static constexpr size_t PAGE_SIZE = 1024 * EVENT_SIZE + sizeof(LinearAllocator::AllocationHeader);

		class const_iterator : public std::iterator<std::forward_iterator_tag, TEvent> {
		public:
			typedef LinearAllocator::AllocationHeader Header;

			explicit const_iterator(const Header *header, size_t count = 0) : 
			header_{ header }, 
			ptr_{ static_cast<const TEvent*>(ptrutil::add(header_, sizeof(Header))) },
			count_{ count } {}

			const_iterator& operator++() {
				ptr_ = static_cast<const TEvent*>(ptrutil::add(ptr_, EVENT_SIZE));
				count_++;
				if (ptrutil::add(ptr_, EVENT_SIZE) > ptrutil::add(header_, header_->size)) {
					header_ = static_cast<const Header*>(header_->nextBlock);
					ptr_ = static_cast<const TEvent*>(ptrutil::add(header_, sizeof(Header)));
				}

				return *this;
			}

			const_iterator operator++(int) { abort(); }
			bool operator==(const const_iterator& other) const { return count_ == other.count_; }
			bool operator!=(const const_iterator& other) const { return !(*this == other); }
			const TEvent& operator*() const { return *ptr_; }

		private:
			const Header *header_;
			const TEvent *ptr_;
			size_t count_;
		};


		EventQueue() : 
		allocator_{ MemoryManager::inst().allocate(PAGE_SIZE), PAGE_SIZE }, 
		proxy_{ &allocator_, PAGE_SIZE },
		size_{ 0 } {}

		const_iterator begin() const { return const_iterator{ static_cast<const typename const_iterator::Header*>(allocator_.getStart()) }; }
		const_iterator end() const { return const_iterator{ nullptr, size_ }; }

		void operator()(const TEvent& event) {
			TEvent *ptr = proxy_.allocate(1);
			new (ptr) TEvent{ event };
			size_++;
		}

		void clear() {
			allocator_.clear();
			size_ = 0;
		}

	private:
		LinearAllocator allocator_;
		oak_allocator<TEvent> proxy_;
		size_t size_;
	};

}