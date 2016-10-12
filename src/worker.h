#pragma once

#include <condition_variable>
#include <bitset>
#include <thread>
#include <mutex>
#include <deque>

#include "task.h"

namespace oak {

	class Worker {
	public:
		Worker();
		~Worker();

		Worker(const Worker &) = delete;
		void operator=(const Worker &) = delete;

		void addTask(Task &&task);
		void wait();

		void run();

		inline size_t taskCount() const { return tasks_.size(); }
		inline void setBackground(bool back) { background_ = back; }
		inline bool inBackground() const { return background_; }

	private:
		std::mutex tasksMutex_;
		std::future 
		std::condition_variable taskCv_;
		std::deque<Task> tasks_;
		std::thread thread_;

		std::mutex doneMutex_;
		std::condition_variable doneCv_;
		std::bitset<2> flags_;

		bool background_;
	};

}