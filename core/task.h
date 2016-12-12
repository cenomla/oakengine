#pragma once

#include <limits>
#include <functional>
#include <atomic>
#include <thread>

namespace oak {

	using TaskId = uint32_t;

	struct Task {
		static constexpr TaskId INVALID_ID = std::numeric_limits<uint32_t>::max();
		static constexpr uint8_t USE_THREAD_AFFINITY = 0x01;
		static constexpr uint8_t AVALIABLE = 0x02;

		Task(TaskId i, int32_t p) : id{ i }, parent{ INVALID_ID }, dependency{ INVALID_ID }, workItems{ 0 }, priority{ p }, flags{ 0 } {}
		Task(TaskId i, std::function<void ()> &&w, int32_t p) : 
			id{ i }, parent{ INVALID_ID }, dependency{ INVALID_ID }, workItems{ 1 }, priority{ p }, run{ w }, flags{ AVALIABLE } {}

		Task(Task &&other) : id{ other.id }, parent{ other.parent }, dependency{ other.dependency },
		workItems{ other.workItems.load() }, priority{ other.priority }, run{ std::move(other.run) }, flags{ other.flags.load() } {
			other.id = INVALID_ID;
			other.parent = INVALID_ID;
			other.dependency = INVALID_ID;
			other.workItems = 0;
			other.priority = 0;
			other.run = {};
			other.flags = 0;
		}

		void operator=(Task &&other) {
			id = other.id;
			parent = other.parent;
			dependency = other.dependency;
			workItems = other.workItems.load();
			priority = other.priority;
			run = std::move(other.run);
			flags = other.flags.load();

			other.id = INVALID_ID;
			other.parent = INVALID_ID;
			other.dependency = INVALID_ID;
			other.workItems = 0;
			other.priority = 0;
			other.run = {};
			other.flags = 0;
		}

		TaskId id;
		TaskId parent;
		TaskId dependency;
		std::atomic<uint32_t> workItems;
		int32_t priority;
		std::thread::id threadAffinity;
		std::function<void ()> run;
		std::atomic<uint8_t> flags;
	};

	struct TaskCompleteEvent {
		Task &task;
	};

}