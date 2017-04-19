#pragma once

#include <mutex>

#include "util/typeid.h"
#include "container.h"
#include "event_queue.h"

namespace oak {

	namespace detail {
		struct BaseEvent {};
	}

	class EventManager {
	private:
		static EventManager *instance;
	public:
		static EventManager& inst() { return *instance; }

		EventManager();
		~EventManager();

		template<class TEvent>
		void addQueue() {
			size_t tid = util::type_id<detail::BaseEvent, TEvent>::id;
			void *ptr = oak_allocator.allocate(sizeof(EventQueueBase));
			new (ptr) EventQueue<TEvent>{};
			queues_[tid] = static_cast<EventQueueBase*>(ptr);
		}

		template<class TEvent>
		EventQueue<TEvent>& getQueue() {
			size_t tid = util::type_id<detail::BaseEvent, TEvent>::id;
			return *static_cast<EventQueue<TEvent>*>(queues_[tid]);
		}

		void clear();
	private:
		oak::vector<EventQueueBase*> queues_;
	};

}