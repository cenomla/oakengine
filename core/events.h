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

	struct CharEvent {
		uint32_t codepoint;
	};

	struct WindowCreateEvent {
		GLFWwindow *window;
	};

	struct WindowResizeEvent {
		int width;
		int height;
	};

	struct EntityActivateEvent {
		Entity entity;
	};

	struct EntityDeactivateEvent {
		Entity entity;
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

	void pup(Puper& puper, QuitEvent& data, const ObjInfo& info);
	void pup(Puper& puper, KeyEvent& data, const ObjInfo& info);
	void pup(Puper& puper, ButtonEvent& data, const ObjInfo& info);
	void pup(Puper& puper, MouseMoveEvent& data, const ObjInfo& info);
	void pup(Puper& puper, CharEvent& data, const ObjInfo& info);
	void pup(Puper& puper, EntityActivateEvent& data, const ObjInfo& info);
	void pup(Puper& puper, EntityDeactivateEvent& data, const ObjInfo& info);
	void pup(Puper& puper, EntityCollisionEvent& data, const ObjInfo& info);

}