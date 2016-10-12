#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "util/typeid.h"

namespace oak {

	class EventChannelBase {
	public:
		virtual ~EventChannelBase() {}
	};

	template<typename TEvent>
	class EventChannel : public EventChannelBase {
	public:

		template<typename TListener> 
		void add(TListener&& listener) {
			listeners_.push_back({ util::type_id<BaseEvtListener, TListener>::id, listener });
		}

		template<typename TListener>
		void remove(TListener&& listener) {
			size_t tid = util::type_id<BaseEvtListener, TListener>::id;
			listeners_.erase(std::remove_if(std::begin(listeners_), std::end(listeners_), [tid](const Listener &elem){ return elem.id == tid; }), std::end(listeners_));
		}
		
		inline void emitEvent(const TEvent& event) const {
			for (const auto &listener : listeners_) {
				listener.func(event);
			}
		}

	private:
		struct BaseEvtListener {};

		struct Listener {
			size_t id;
			std::function<void (const TEvent&)> func;
		};
		std::vector<Listener> listeners_;
	};

}