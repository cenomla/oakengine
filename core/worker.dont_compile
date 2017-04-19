#include "worker.h"

#include "engine.h"

namespace oak {

	Worker::~Worker() {
		if (thread_.joinable()) {
			thread_.join();
		}
	}

	void Worker::init(TaskManager *taskManager) {
		taskManager_ = taskManager;
		thread_ = std::thread{ [this](){ run(); } };
	}

	void Worker::run() {
		running_ = true;
		while (running_) {
			if (taskManager_->taskCount_ > 0) {
				auto task = taskManager_->grabTask();
				if (task != nullptr) {
					task->run();
					Engine::inst().getEventManager().emitEvent(TaskCompleteEvent{ *task });
				} else {
					std::unique_lock<std::mutex> lock{ taskManager_->cvMutex_ };
					taskManager_->taskCv_.wait(lock);
				}
			} else {
				std::unique_lock<std::mutex> lock{ taskManager_->cvMutex_ };
				while (running_ && taskManager_->taskCount_ == 0) {
					taskManager_->taskCv_.wait(lock);
				}
			}
		}
	}

	void Worker::quit() {
		running_ = false;
	}

}