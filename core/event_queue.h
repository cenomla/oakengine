#pragma once

#include <functional>
#include <vector>
#include <mutex>

namespace oak {

	template <typename TEvent>
	class EventQueue {
	public:
		
		template<typename TFunc>
		void setCallback(TFunc&& callback) {
			callback_ = std::forward<TFunc>(callback);
		}

		void swap() {
			std::lock_guard<std::mutex> lock{ eventsMutex_ };
			std::swap(events_[0], events_[1]);
		}

		void processEvents() {
			for (const auto &event : events_[0]) {
				callback_(event);
			}
		}

		void clear() {
			events_[0].clear();
		}

		void operator()(const TEvent& event) {
			std::lock_guard<std::mutex> guard{ eventsMutex_ };
			events_[1].push_back(event);
		}

	private:
		std::function<void (const TEvent&)> callback_;
		std::array<std::vector<TEvent>, 2> events_;
		std::mutex eventsMutex_;
	};

}