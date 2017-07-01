#pragma once

#include <cinttypes>

#include "input.h"

struct GLFWwindow;

namespace oak {

	class Puper;
	struct ObjInfo;
	
	struct WindowCreateEvent {
		GLFWwindow *window;
	};

	struct WindowCloseEvent {
		GLFWwindow *window;
	};

	struct WindowResizeEvent {
		int width, height;
	};

	struct KeyEvent {
		int key, scancode, action, mods;
	};

	struct ButtonEvent {
		int button, action, mods;
	};

	struct CursorEvent {
		float x, y;
	};

	struct TextEvent {
		uint32_t codepoint;
	};

	struct CursorModeEvent {
		CursorMode mode;
	};

	void pup(Puper& puper, WindowCreateEvent& data, const ObjInfo& info);
	void pup(Puper& puper, WindowCloseEvent& data, const ObjInfo& info);
	void pup(Puper& puper, WindowResizeEvent& data, const ObjInfo& info);
	void pup(Puper& puper, KeyEvent& data, const ObjInfo& info);
	void pup(Puper& puper, ButtonEvent& data, const ObjInfo& info);
	void pup(Puper& puper, CursorEvent& data, const ObjInfo& info);
	void pup(Puper& puper, TextEvent& data, const ObjInfo& info);
	

}
