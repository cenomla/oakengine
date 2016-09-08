#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <array>

#include "event_queue.h"
#include "events.h"
#include "worker.h"
#include "task.h"

namespace oak {

	class Engine;

	class TaskManager {
	public:
		TaskManager();
		~TaskManager();

		void init(Engine *engine);

		void run();

		void addTask(Task &&task);

		void quit();

		void operator()(const TaskExitEvent&);


	private:
		std::mutex tasksMutex_;
		std::array<std::queue<Task>, 2> tasks_;

		std::vector<Worker> workers_;

		std::atomic<bool> running_;

	};

}