#pragma once

struct GLFWwindow;

namespace oak {

	struct Task;
	
	struct GameExitEvent {};

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

	struct WindowResizeEvent {
		int width;
		int height;
	};

}