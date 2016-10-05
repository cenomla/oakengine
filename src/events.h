#pragma once

struct GLFWwindow;

namespace oak {

	struct Task;
	
	struct QuitEvent {};

	struct TaskCompletedEvent {
		Task *task;
	};

	struct KeyEvent {
		int key;
		int scancode;
		int action;
		int mods;
	};

	struct WindowCreateEvent {
		GLFWwindow *window;
	};

	struct ResizeEvent {
		int width;
		int height;
	};

}