#pragma once

#include <unordered_map>
#include <mutex>

#include "util/typeid.h"
#include "memory/memory_manager.h"
#include "event_channel.h"

namespace oak {

	class EventManager {
	public:

		~EventManager() {
			for (auto &it : channels_) {
				static_cast<EventChannelBase*>(it.second.ptr)->~EventChannelBase();
				MemoryManager::inst().deallocate(it.second.ptr, it.second.size);
			}
		}

		template <typename TEvent, typename TListener>
		void add(TListener&& listener) {
			size_t tid = util::type_id<BaseEvt, TEvent>::id;
			auto it = channels_.find(tid);
			if (it == std::end(channels_)) {
				size_t size = sizeof(EventChannel<TEvent>);
				const Block &block = { MemoryManager::inst().allocate(size), size };
				auto *channel = new (block.ptr) EventChannel<TEvent>();
				channel->add(listener);
				std::lock_guard<std::mutex> guard{ channelsMutex_ };
				channels_.insert({ tid, block });
			} else {
				static_cast<EventChannel<TEvent>*>(it->second.ptr)->add(listener);
			}
			
		}

		template <typename TEvent, typename TListener>
		void remove(TListener &&listener) {
			std::lock_guard<std::mutex> guard{ channelsMutex_ };
			static_cast<EventChannel<TEvent>*>(channels_.at(util::type_id<BaseEvt, TEvent>::id).ptr)->remove(listener);
		}

		template <typename TEvent>
		void emitEvent(const TEvent &event) const {
			size_t tid = util::type_id<BaseEvt, TEvent>::id;
			auto it = channels_.find(tid);
			if (it != std::end(channels_)) {
				static_cast<EventChannel<TEvent>*>(channels_.at(tid).ptr)->emitEvent(event);
			}
		}

	private:
		struct BaseEvt {};

		std::mutex channelsMutex_;
		std::unordered_map<size_t, Block> channels_;
	};

}