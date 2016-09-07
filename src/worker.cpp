#include "worker.h"

namespace oak {

	Worker::Worker() : flags_{ 0 } {}

	Worker::~Worker() {
		{
			std::lock_guard<std::mutex> lock{ tasksMutex_ };
			flags_[1] = true;
		}
		taskCv_.notify_one();
		if (thread_.joinable()) {
			thread_.join();
		}
	}

	void Worker::addTask(Task *task) {
		{
			std::lock(tasksMutex_, doneMutex_);
			std::lock_guard<std::mutex> lock1{ tasksMutex_, std::adopt_lock };
			std::lock_guard<std::mutex> lock2{ doneMutex_, std::adopt_lock };
			tasks_.push_back(task);
			flags_[0] = true;
		}

		taskCv_.notify_one();
		if (!thread_.joinable()) {
			thread_ = std::move(std::thread{ [this](){ this->run(); } });
		}
	}

	void Worker::wait() {
		std::unique_lock<std::mutex> lock{ doneMutex_ };
		while (flags_[0]) {
			doneCv_.wait(lock);
		}
	}

	void Worker::run() {
		while (true) {
			Task *task;
			{
				std::lock_guard<std::mutex> lock{ tasksMutex_ };
				task = tasks_.front();
			}
			task->run();
			{
				std::lock_guard<std::mutex> lock{ tasksMutex_ };
				tasks_.pop_front();
			}
			if (tasks_.size() == 0) {
				{
					std::lock_guard<std::mutex> lock{ doneMutex_ };
					flags_[0] = false;
				}
				doneCv_.notify_one();
				std::unique_lock<std::mutex> lock{ tasksMutex_ };
				taskCv_.wait(lock);
				if (flags_[1]) {
					break;
				}
			}

		}
	}

}