#pragma once

#include <functional>
#include <vector>
#include <mutex>

namespace oak {

	template <typename TEvent>
	class EventQueue {
	public:
		EventQueue(const std::function<void (const TEvent&)> &callback) : callback_{ callback } {}

		void processEvents() {
			for (auto &event : events_) {
				callback_(event);
			}

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