#pragma once

namespace oak {

	struct Task;
	
	struct TaskExitEvent {};

	struct TaskCompletedEvent {
		Task *task;
	};

}