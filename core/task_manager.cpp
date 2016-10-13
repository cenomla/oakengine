#include "task_manager.h"

#include <iostream>

#include "engine.h"

namespace oak {

	TaskManager::TaskManager() : workers_{ std::thread::hardware_concurrency() }, running_{ false } {

	}

	TaskManager::~TaskManager() {
		
	}

	void TaskManager::init() {
		std::cout << "hardware supported threads: " << workers_.size() << std::endl;

		for (auto&& worker : workers_) {
			worker.init();
		}

		Engine::inst().getEventManager().add<GameExitEvent>(std::ref(*this));
	}

	void TaskManager::destroy() {
		//wait for all workers to complete their tasks
		for (auto&& worker : workers_) {
			worker.quit();
		}
	}

	void TaskManager::run() {
		std::swap(tasks_[0], tasks_[1]);
		running_.store(true);
		while (running_.load()) {
			//pop a task from the front of the queue
			Task task = std::move(tasks_[0].front());
			tasks_[0].pop();

			//if the task is supposed to loop add it to the next queue
			if (task.flags[Task::LOOP]) {
				addTask(task);
			}
			
			task.run();

			if (tasks_[0].empty()) {
				std::lock_guard<std::mutex> lock{ tasksMutex_ };
				std::swap(tasks_[0], tasks_[1]);
				if (tasks_[0].empty()) {
					running_ = false;
				}
			}
		}
	}

	void TaskManager::addTask(Task task) {
		//add the task to a worker

		if (task.flags[Task::MULTI_THREAD]) {
			size_t leastTaskCount = 99;
			int index = -1;
			for (size_t i = 0; i < workers_.size(); i++) {
				Worker &worker = workers_[i];
				if (worker.taskCount() < leastTaskCount) {
					leastTaskCount = worker.taskCount();
					index = static_cast<int>(i);
				}
			}

			if (index != -1) {
				workers_[index].addTask(std::move(task));	
			}
		} else {
			std::lock_guard<std::mutex> lock{ tasksMutex_ };
			tasks_[1].push(std::move(task));
		}
	}

	void TaskManager::operator()(const GameExitEvent&) {
		running_ = false;
	}

}