#pragma once

#include <thread>
#include <atomic>

namespace oak {
	
	class TaskManager;

	class Worker {
	public:
		~Worker();

		void init(TaskManager *taskManager);

		void run();
		void quit();
	private:
		std::thread thread_;
		std::atomic<bool> running_;

		TaskManager *taskManager_;
	};

}