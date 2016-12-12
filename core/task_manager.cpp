#include "task_manager.h"

#include <algorithm>
#include <iostream>
#include <chrono>
#include <limits>

#include "engine.h"

namespace oak {

	TaskManager::TaskManager(size_t threadCount) : taskId_{ 0 }, taskCount_{ 0 }, isDone_{ false }, workers_{ std::thread::hardware_concurrency() - threadCount } {

	}

	TaskManager::~TaskManager() {}

	void TaskManager::init() {
		std::cout << "num workers: " << workers_.size() << std::endl;

		Engine::inst().getEventManager().add<TaskCompleteEvent>(std::ref(*this));
		for (auto&& worker : workers_) {
			worker.init(this);
		}
	}

	void TaskManager::destroy() {
		//tell the workers to quit their tasks
		for (auto&& worker : workers_) {
			worker.quit();
		}
		taskCv_.notify_all();
	}

	void TaskManager::kick() {
		taskCv_.notify_all();
	}

	void TaskManager::wait() {
		while (!isDone_) {
			//Task task = std::move(grabTask());
			//if (task.id != Task::INVALID_ID) {
				//task.run();
				//Engine::inst().getEventManager().emitEvent(TaskCompleteEvent{ std::move(task) });
			//}

			//std::this_thread::sleep_for(std::chrono::milliseconds{ 200 });

		}
	}

	void TaskManager::swap() {
		std::lock_guard<std::mutex> lock{ tasksMutex_ };
		std::swap(tasks_[0], tasks_[1]);
		tasks_[0].clear();
		taskId_ = 0;
		isDone_ = false;
	}

	void TaskManager::addTask(Task &&task) {
		std::lock_guard<std::mutex> lock{ tasksMutex_ };
		tasks_[0].push_back(std::move(task));
		taskCount_ ++;
	}

	Task* TaskManager::grabTask() {
		std::lock_guard<std::mutex> lock{ tasksMutex_ };
		for (auto& task : tasks_[1]) {
			if (task.workItems > 0) {
				//if the task has a dependency make sure it is done first
				if (task.dependency != Task::INVALID_ID && tasks_[1][task.dependency].workItems > 0) {
					continue;
				}
				//if the task is avaliable, has work and has no children then return the task
				if ((task.flags.load() & Task::AVALIABLE) == Task::AVALIABLE && task.workItems == 1) {
					//make sure the task is not avaliable
					task.flags.fetch_and(~Task::AVALIABLE);
					return &task;
				}
			}
		}
		return nullptr;
	}

	void TaskManager::operator()(const TaskCompleteEvent &evt) {
		evt.task.workItems--;
		TaskId parent = evt.task.parent;
		if (parent != Task::INVALID_ID) {
			tasks_[1][parent].workItems--;
		}
		taskCount_ --;
		if (taskCount_ > 0) {
			taskCv_.notify_all();
		} else {
			isDone_ = true;
		}
	}

	void TaskManager::depends(TaskId task, TaskId dependency) {
		tasks_[0][task].dependency = dependency;
	}

	void TaskManager::addChild(TaskId parent, TaskId child) {
		tasks_[0][parent].workItems++;
		tasks_[0][child].parent = parent;
	}

}