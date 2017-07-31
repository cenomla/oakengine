#include "input_events.h"

#include "pup.h"

namespace oak {

	void pup(Puper& puper, WindowCreateEvent& data, const ObjInfo& info) {
		pup(puper, data.window, ObjInfo::make<void*>(&info, "window"));
	}

	void pup(Puper& puper, WindowCloseEvent& data, const ObjInfo& info) {
		pup(puper, data.window, ObjInfo::make<void*>(&info, "window"));
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

}
