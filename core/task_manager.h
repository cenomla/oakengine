#pragma once

#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "container.h"
#include "event_queue.h"
#include "events.h"
#include "worker.h"
#include "task.h"

namespace oak {

	class TaskManager {
	public:
		friend class Worker;

		TaskManager(size_t threadCount);
		~TaskManager();

		void init();
		void destroy();

		void kick();
		void wait();
		void swap();

		template<class... TArgs>
		TaskId createTask(TArgs&&... args) {
			TaskId id = taskId_++;
			addTask({ id, std::forward<TArgs>(args)... });
			return id;
		}

		void addTask(Task &&task);
		Task* grabTask();

		void operator()(const TaskCompleteEvent &evt);

		void depends(TaskId task, TaskId dependency);
		void addChild(TaskId parent, TaskId child);

	private:
		std::mutex tasksMutex_;
		std::array<oak::vector<Task>, 2> tasks_;
		std::atomic<TaskId> taskId_;
		std::atomic<size_t> taskCount_;
		std::atomic<bool> isDone_;
		
		std::mutex cvMutex_;
		std::condition_variable taskCv_;

		oak::vector<Worker> workers_;
	};

}