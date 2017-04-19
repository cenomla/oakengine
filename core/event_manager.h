#pragma once

#include <mutex>

#include "util/typeid.h"
#include "container.h"
#include "oak_alloc.h"
#include "event_queue.h"

namespace oak {

	namespace detail {
		struct BaseEvent {};
	}

	class EventManager {
	private:
		static EventManager *instance;
	public:
		static EventManager* inst() { return instance; }

		EventManager();
		~EventManager();

		template<class TEvent>
		void addQueue() {
			size_t tid = util::type_id<detail::BaseEvent, TEvent>::id;
			size_t size = sizeof(EventQueue<TEvent>);
			void *ptr = oak_allocator.allocate(size);
			new (ptr) EventQueue<TEvent>{};
			queues_[tid] = ptr;
		}

		template<class TEvent>
		const EventQueue<TEvent>& getQueue() {
			size_t tid = util::type_id<detail::BaseEvent, TEvent>::id;
			return *static_cast<EventQueue<TEvent>*>(queues_[tid]);
		}

		void clear();
	private:
		oak::vector<void*> queues_;
	};

}