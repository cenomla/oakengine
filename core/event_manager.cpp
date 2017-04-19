#include "event_manager.h"

namespace oak {

	EventManager *EventManager::instance = nullptr;


	EventManager::EventManager() {
		instance = this;
	}

	EventManager::~EventManager() {
		size_t size = sizeof(EventChannel<void>);
		for (auto& queue : queues_) {
			queue->~EventQueue<void>();
			oak_allocator.deallocate(queue, size);
		}
		instance = nullptr;
	}

	void EventManager::clear() {
		for (auto& queue : queues_) {
			static_cast<EventQueue<void>*>(queue)->clear();
		}
	}

}
