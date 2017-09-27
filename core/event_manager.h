#pragma once

#include <mutex>

#include "oak_assert.h"

#include "util/type_id.h"
#include "container.h"
#include "event_queue.h"
#include "event.h"

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

		void init();

		void addQueue(const TypeInfo *tinfo);
		EventQueue& getQueue(const TypeInfo *tinfo);

		void clear();
	private:
		oak::vector<EventQueue*> queues_;
	};

}
