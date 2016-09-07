#pragma once

#include <algorithm>
#include <functional>
#include <vector>

namespace oak {

	class EventChannelBase {
	public:
		virtual ~EventChannelBase() {}
	};

	template<typename TEvent>
	class EventChannel : public EventChannelBase {
	public:

		template<typename TListener>
		void add(TListener *listener) {
			listeners_.push_back( { listener, [listener](const TEvent &evt){ (*listener)(evt); } });
		}

		template<typename TListener>
		void remove(TListener *listener) {
			listeners_.erase(std::remove_if(std::begin(listeners_), std::end(listeners_), [listener](const Listener &elem){ return elem.id == listener; }), std::end(listeners_));
		}
		
		void emitEvent(const TEvent& event) const {
			for (const auto &listener : listeners_) {
				listener.func(event);
			}
		}

	private:
		struct Listener {
			void *id;
			std::function<void (const TEvent&)> func;
		};
		std::vector<Listener> listeners_;
	};

}