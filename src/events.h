#pragma once

namespace oak {

	struct Task;
	
	struct QuitEvent {};

	struct TaskCompletedEvent {
		Task *task;
	};

}