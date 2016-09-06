#pragma once

#include <queue>
#include <atomic>

#include "event_queue.h"
#include "task.h"

namespace oak {

	struct TaskExitEvent {};

	struct TaskCompletedEvent {
		Task *task;
	};

	class Engine;

	class TaskManager {
	public:
		TaskManager();
		~TaskManager();

		void init(Engine *engine);

		void run();

		void addTask(Task *task);

		void handleCompletedTask(const TaskCompletedEvent &evt);
		void quit();


	private:
		std::queue<Task*> tasks_;

		std::atomic<bool> running_;

		EventQueue<TaskExitEvent> exitQueue_;
		EventQueue<TaskCompletedEvent> completedQueue_;

	};

}