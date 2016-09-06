#pragma once

#include <unordered_map>
#include <mutex>

#include "util/typeid.h"
#include "event_channel.h"

namespace oak {

	class EventManager {
	public:

		~EventManager() {
			for (auto &it : channels_) {
				delete it.second;
			}
		}

		template <typename TEvent, typename TListener>
		void add(TListener *listener) {
			auto channel = new EventChannel<TEvent>{};
			channel->add(listener);
			std::lock_guard<std::mutex> guard{ channelsMutex_ };
			channels_.insert({ util::TypeId<BaseEvt>::id<TEvent>(), channel });
		}

		template <typename TEvent, typename TListener>
		void remove(TListener *listener) {
			std::lock_guard<std::mutex> guard{ channelsMutex_ };
			static_cast<EventChannel<TEvent>*>(channels_.at(util::TypeId<BaseEvt>::id<TEvent>()))->remove(listener);
		}

		template <typename TEvent>
		void emitEvent(const TEvent &event) const {
			static_cast<EventChannel<TEvent>*>(channels_.at(util::TypeId<BaseEvt>::id<TEvent>()))->emitEvent(event);
		}

	private:
		struct BaseEvt {};

		std::mutex channelsMutex_;
		std::unordered_map<size_t, void*> channels_;
	};

}