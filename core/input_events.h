#pragma once

#include <cinttypes>

#include "input.h"
#include "type_info.h"

struct GLFWwindow;

namespace oak {

	class Puper;
	struct ObjInfo;
	
	struct WindowCreateEvent {
		static const TypeInfo typeInfo;
		GLFWwindow *window;
	};

	struct WindowCloseEvent {
		static const TypeInfo typeInfo;
		GLFWwindow *window;
	};

	struct WindowResizeEvent {
		static const TypeInfo typeInfo;
		int width, height;
	};

	struct FrameSizeEvent {
		static const TypeInfo typeInfo;
		int width, height;
	};

	struct KeyEvent {
		static const TypeInfo typeInfo;
		int key, scancode, action, mods;
	};

	struct ButtonEvent {
		static const TypeInfo typeInfo;
		int button, action, mods;
	};

	struct CursorEvent {
		static const TypeInfo typeInfo;
		float x, y;
	};

	struct TextEvent {
		static const TypeInfo typeInfo;
		uint32_t codepoint;
	};

	struct CursorModeEvent {
		static const TypeInfo typeInfo;
		CursorMode mode;
	};

	void pup(Puper& puper, WindowCreateEvent& data, const ObjInfo& info);
	void pup(Puper& puper, WindowCloseEvent& data, const ObjInfo& info);
	void pup(Puper& puper, WindowResizeEvent& data, const ObjInfo& info);
	void pup(Puper& puper, FrameSizeEvent& data, const ObjInfo& info);
	void pup(Puper& puper, KeyEvent& data, const ObjInfo& info);
	void pup(Puper& puper, ButtonEvent& data, const ObjInfo& info);
	void pup(Puper& puper, CursorEvent& data, const ObjInfo& info);
	void pup(Puper& puper, TextEvent& data, const ObjInfo& info);
	void pup(Puper& puper, CursorModeEvent& data, const ObjInfo& info);
	

}
