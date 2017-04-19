#include "event_manager.h"

namespace oak {

	EventManager *EventManager::instance = nullptr;


	EventManager::EventManager() {
		instance = this;
	}

	EventManager::~EventManager() {
		for (auto& queue : queues_) {
			queue->~EventQueueBase();
			oak_allocator.deallocate(queue, sizeof(EventQueueBase));
		}
		instance = nullptr;
	}

	void EventManager::clear() {
		for (auto& queue : queues_) {
			queue->clear();
		}
	}

}
