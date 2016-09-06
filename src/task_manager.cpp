#include "task_manager.h"

#include "engine.h"

namespace oak {

	TaskManager::TaskManager() : 
		running_{ false }, 
		exitQueue_{ [this](const TaskExitEvent &evt) { this->quit(); } }, 
		completedQueue_{ [this](const TaskCompletedEvent &evt) { this->handleCompletedTask(evt); } } {

	}

	TaskManager::~TaskManager() {

	}

	void TaskManager::init(Engine *engine) {
		engine->getEventManager().add<TaskExitEvent>(&exitQueue_);
		engine->getEventManager().add<TaskCompletedEvent>(&completedQueue_);
	}

	void TaskManager::run() {
		running_.store(true);
		while (running_.load()) {
			Task *task = tasks_.front();
			tasks_.pop();
			task->run();
			Engine::inst().getEventManager().emitEvent(TaskCompletedEvent{ task });
			completedQueue_.processEvents();
			exitQueue_.processEvents();
		}
	}

	void TaskManager::addTask(Task *task) {
		tasks_.push(task);
	}

	void TaskManager::handleCompletedTask(const TaskCompletedEvent &evt) {
		if (evt.task->flags[0]) {
			tasks_.push(evt.task);
		}
		if (tasks_.empty()) {
			Engine::inst().getEventManager().emitEvent(TaskExitEvent{});
		}
	}

	void TaskManager::quit() {
		running_.store(false);
	}

}