#include "worker.h"

#include "engine.h"

namespace oak {

	Worker::~Worker() {
		if (thread_.joinable()) {
			thread_.join();
		}
	}

	void Worker::init() {
		thread_ = std::thread{ [this](){ run(); } };
		taskCount_ = 0;
	}

	void Worker::addTask(Task &&task) {
		{
			std::lock_guard<std::mutex> lock1{ tasksMutex_ };
			tasks_.push_back(std::move(task));
			taskCount_++;
		}

		taskCv_.notify_one();
	}

	void Worker::run() {
		running_ = true;
		while (running_) {
			//execute tasks until done
			if (taskCount_ > 0) {
				Task task;
				//get the oldest task
				{
					std::lock_guard<std::mutex> lock{ tasksMutex_ };
					task = std::move(tasks_.front());
					tasks_.pop_front();
				}

				//run the task
				task.run();
				taskCount_--;

				//if the task is supposed to loop then add it to the task manager
				if (task.flags[Task::LOOP]) {
					Engine::inst().getTaskManager().addTask(std::move(task));
				}
			}

			//wait for task to be avalable
			if (taskCount_ == 0) {
				std::unique_lock<std::mutex> lock{ cvMutex_ };
				while (tasks_.empty() && running_) {
					taskCv_.wait(lock);
				}
			}
		}
	}

	void Worker::quit() {
		running_ = false;
		taskCv_.notify_all();
	}

}