#pragma once

#include <condition_variable>
#include <thread>
#include <mutex>
#include <deque>
#include <atomic>

#include "task.h"

namespace oak {

	class Worker {
	public:

		~Worker();

		//Worker(const Worker &) = delete;
		//void operator=(const Worker &) = delete;
		void init();

		void addTask(Task &&task);
		void run();
		void quit();

		inline size_t taskCount() const { return tasks_.size(); }
	private:
		std::thread thread_;

		std::mutex tasksMutex_;
		std::mutex cvMutex_;
		std::condition_variable taskCv_;

		std::deque<Task> tasks_;

		std::atomic<bool> running_;
	};

}