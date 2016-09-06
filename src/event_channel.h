#pragma once

#include <functional>
#include <unordered_map>

namespace oak {

	template<typename TEvent>
	class EventChannel {
	public:
		EventChannel() {};

		template<typename TListener>
		void add(TListener *listener) {
			listeners_.insert({ listener, [listener](const TEvent& evt) { (*listener)(evt); } });
		}

		template<typename TListener>
		void remove(TListener *listener) {
			listeners_.erase(listener);
		}
		
		void emitEvent(const TEvent& event) const {
			for (const auto &it : listeners_) {
				it.second(event);
			}
		}

	private:
		std::unordered_map<void*, std::function<void (const TEvent&)>> listeners_;
	};

}