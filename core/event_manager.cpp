#include "event_manager.h"

namespace oak {

	EventManager *EventManager::instance = nullptr;


	EventManager::EventManager() {
		oak_assert(instance == nullptr);
		instance = this;
	}

	EventManager::~EventManager() {
		for (auto& queue : queues_) {
			queue->~EventQueue();
			oak_allocator.deallocate(queue, sizeof(EventQueue));
		}
		queues_.clear();
		instance = nullptr;
	}

	void EventManager::init() {
		for (auto type : EventTypeManager::inst().getTypes()) {
			addQueue(type);
		}
	}

	void EventManager::addQueue(const TypeInfo *tinfo) {
		size_t tid = tinfo->id;
		auto *ptr = static_cast<EventQueue*>(oak_allocator.allocate(sizeof(EventQueue)));
		new (ptr) EventQueue{ tinfo };
		if (queues_.size() <= tid) {
			queues_.resize(tid + 1);
		}
		queues_[tid] = ptr;
	}

	EventQueue& EventManager::getQueue(const TypeInfo *tinfo) {
		size_t tid = tinfo->id;
		return *queues_[tid];
	}

	void EventManager::clear() {
		for (auto& queue : queues_) {
			queue->clear();
		}
	}

}
