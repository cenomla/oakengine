#pragma once

#include <glm/glm.hpp>
#include "entity.h"

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

	struct ButtonEvent {
		int button;
		int action;
		int mods;
	};

	struct MouseMoveEvent {
		int x, y;
	};

	struct WindowCreateEvent {
		GLFWwindow *window;
	};

	struct WindowResizeEvent {
		int width;
		int height;
	};

	struct EntityCollisionEvent {
		Entity entity;
		Entity other;
		glm::vec2 normal;
		float depth;
	};

	struct PositionSetEvent {
		Entity entity;
		glm::vec3 pos;
	};

	struct PositionMoveEvent {
		Entity entity;
		glm::vec3 deltaPos;
	};

	struct ApplyForceEvent {
		Entity entity;
		glm::vec2 force;
	};

}