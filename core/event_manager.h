#pragma once

#include <mutex>

#include "oak_assert.h"

#include "util/type_id.h"
#include "container.h"
#include "event_queue.h"
#include "events.h"

namespace oak {

	class EventManager {
	private:
		static EventManager *instance;
	public:
		inline static EventManager& inst() { 
			oak_assert(instance != nullptr);
			return *instance; 
		}

		EventManager();
		~EventManager();

		template<class TEvent>
		void addQueue() {
			size_t tid = TEvent::typeInfo.id;
			void *ptr = oak_allocator.allocate(sizeof(EventQueueBase));
			new (ptr) EventQueue<TEvent>{};
			if (queues_.size() <= tid) {
				queues_.resize(tid + 1);
			}
			queues_[tid] = static_cast<EventQueueBase*>(ptr);
		}

		template<class TEvent>
		EventQueue<TEvent>& getQueue() {
			size_t tid = TEvent::typeInfo.id;
			return *static_cast<EventQueue<TEvent>*>(queues_[tid]);
		}

		void clear();
	private:
		oak::vector<EventQueueBase*> queues_;
	};

}
