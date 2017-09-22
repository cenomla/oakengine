#include "input_events.h"

namespace oak {

	const TypeInfo WindowCreateEvent::typeInfo = makeEventInfo<WindowCreateEvent>("window_created");
	const TypeInfo WindowCloseEvent::typeInfo = makeEventInfo<WindowCloseEvent>("window_closed");
	const TypeInfo WindowResizeEvent::typeInfo = makeEventInfo<WindowResizeEvent>("window_resized");
	const TypeInfo FrameSizeEvent::typeInfo = makeEventInfo<FrameSizeEvent>("frame_resized");
	const TypeInfo KeyEvent::typeInfo = makeEventInfo<KeyEvent>("key_changed");
	const TypeInfo ButtonEvent::typeInfo = makeEventInfo<ButtonEvent>("button_changed");
	const TypeInfo CursorEvent::typeInfo = makeEventInfo<CursorEvent>("cursor_moved");
	const TypeInfo TextEvent::typeInfo = makeEventInfo<TextEvent>("text_input");
	const TypeInfo CursorModeEvent::typeInfo = makeEventInfo<CursorModeEvent>("cusor_mode");

	void pup(Puper& puper, WindowCreateEvent& data, const ObjInfo& info) {
		//pup(puper, data.window, ObjInfo::make<void*>(&info, "window"));
	}

	void pup(Puper& puper, WindowCloseEvent& data, const ObjInfo& info) {
		//pup(puper, data.window, ObjInfo::make<void*>(&info, "window"));
	}

	void pup(Puper& puper, WindowResizeEvent& data, const ObjInfo& info) {
		pup(puper, data.width, ObjInfo::make<int>(&info, "width"));
		pup(puper, data.height, ObjInfo::make<int>(&info, "height"));
	}

	void pup(Puper& puper, FrameSizeEvent& data, const ObjInfo& info) {
		pup(puper, data.width, ObjInfo::make<int>(&info, "width"));
		pup(puper, data.height, ObjInfo::make<int>(&info, "height"));
	}

	void pup(Puper& puper, KeyEvent& data, const ObjInfo& info) {
		pup(puper, data.key, ObjInfo::make<int>(&info, "key"));		
		pup(puper, data.scancode, ObjInfo::make<int>(&info, "scancode"));
		pup(puper, data.action, ObjInfo::make<int>(&info, "action"));
		pup(puper, data.mods, ObjInfo::make<int>(&info, "mods"));
	}

	void pup(Puper& puper, ButtonEvent& data, const ObjInfo& info) {
		pup(puper, data.button, ObjInfo::make<int>(&info, "button"));
		pup(puper, data.action, ObjInfo::make<int>(&info, "action"));
		pup(puper, data.mods, ObjInfo::make<int>(&info, "mods"));
	}

	void pup(Puper& puper, CursorEvent& data, const ObjInfo& info) {
		pup(puper, data.x, ObjInfo::make<int>(&info, "x"));
		pup(puper, data.y, ObjInfo::make<int>(&info, "y"));
	}

	void pup(Puper& puper, TextEvent& data, const ObjInfo& info) {
		pup(puper, data.codepoint, ObjInfo::make<int>(&info, "codepoint"));
	}

	void pup(Puper& puper, CursorModeEvent& data, const ObjInfo& info) {
		int m = static_cast<int>(data.mode);
		pup(puper, m, ObjInfo::make<int>(&info, "mode"));
		data.mode = static_cast<CursorMode>(m);
	}

}
