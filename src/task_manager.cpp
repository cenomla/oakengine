#include "task_manager.h"

#include <iostream>

#include "engine.h"

namespace oak {

	TaskManager::TaskManager() : workers_{ std::thread::hardware_concurrency() }, running_{ false } {

	}

	TaskManager::~TaskManager() {
		
	}

	void TaskManager::init(Engine *engine) {
		std::cout << "hardware supported threads: " << workers_.size() << std::endl;
		for (size_t i = 0; i < workers_.size(); i++) {
			workers_[i].setBackground(i < workers_.size()/2 ? false : true);
		}
		engine->getEventManager().add<QuitEvent>(std::ref(*this));
	}

	void TaskManager::destroy() {
		//wait for all workers to complete their tasks
		for (auto &worker : workers_) {
			worker.wait();
		}
	}

	void TaskManager::run() {
		std::swap(tasks_[0], tasks_[1]);
		running_.store(true);
		while (running_.load()) {
			//pop a task from the front of the queue
			Task task = tasks_[0].front();
			tasks_[0].pop();
			//if the task is supposed to loop add it to the next queue
			if (task.flags[Task::LOOP]) {
				std::lock_guard<std::mutex> lock{ tasksMutex_ };
				tasks_[1].push(task);
			}
			//if the task is allowed to execute concurrently find the least busy worker thread and add the task to the worker
			if (task.flags[Task::MULTI_THREAD]) {
				size_t leastTaskCount = 99;
				int index = -1;
				for (size_t i = 0; i < workers_.size(); i++) {
					Worker &worker = workers_[i];
					if (worker.inBackground() == task.flags[Task::BACKGROUND] && worker.taskCount() < leastTaskCount) {
						leastTaskCount = worker.taskCount();
						index = static_cast<int>(i);
					}
				}

				if (index != -1) {
					workers_[index].addTask(std::move(task));	
				}
			} else {
				task.run();
			}

			//if we have run out of tasks then wait till all tasks are completed and then swap the read/write task queues
			if (tasks_[0].empty()) {
				for (auto &worker : workers_) {
					if (!worker.inBackground()) {
						worker.wait();//wait for the worker if it is not in the background
					}
				}
				//swap tasks
				std::lock_guard<std::mutex> lock{ tasksMutex_ };
				std::swap(tasks_[0], tasks_[1]);
				if (tasks_[0].empty()) {
					//if there are no tasks then quit
					quit();
				}
			}
		}
	}

	void TaskManager::addTask(Task &&task) {
		std::lock_guard<std::mutex> lock{ tasksMutex_ };
		tasks_[1].push(std::move(task));
	}

	void TaskManager::quit() {
		running_.store(false);
	}

	void TaskManager::operator()(const QuitEvent&) {
		quit();
	}

}