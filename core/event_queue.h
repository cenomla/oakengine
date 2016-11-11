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

		void processEvents() {
			for (auto &event : events_) {
				callback_(event);
			}
		}

		void clear() {
			events_.clear();
		}

		void operator()(const TEvent& event) {
			std::lock_guard<std::mutex> guard{ eventsMutex_ };
			events_.push_back(event);
		}

	private:
		std::function<void (const TEvent&)> callback_;
		std::vector<TEvent> events_;
		std::mutex eventsMutex_;
	};

}