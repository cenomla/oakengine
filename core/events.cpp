#include "events.h"

#include "pup.h"

namespace oak {

	void pup(Puper& puper, QuitEvent& data, const ObjInfo& info) {
		
	}

	void pup(Puper& puper, KeyEvent& data, const ObjInfo& info) {
		pup(puper, data.key, info + ObjInfo{ "key" });
		pup(puper, data.scancode, info + ObjInfo{ "scancode" });
		pup(puper, data.action, info + ObjInfo{ "action" });
		pup(puper, data.mods, info + ObjInfo{ "mods" });
	}

	void pup(Puper& puper, ButtonEvent& data, const ObjInfo& info) {
		pup(puper, data.button, info + ObjInfo{ "button" });
		pup(puper, data.action, info + ObjInfo{ "action" });
		pup(puper, data.mods, info + ObjInfo{ "mods" });
	}

	void pup(Puper& puper, MouseMoveEvent& data, const ObjInfo& info) {
		pup(puper, data.x, info + ObjInfo{ "x" });
		pup(puper, data.y, info + ObjInfo{ "y" });
	}

	void pup(Puper& puper, CharEvent& data, const ObjInfo& info) {
		pup(puper, data.codepoint, info + ObjInfo{ "codepoint" });
	}

	void pup(Puper& puper, EntityCreateEvent& data, const ObjInfo& info) {
		pup(puper, data.entity, info + ObjInfo{ "entity" });
	}

	void pup(Puper& puper, EntityDestroyEvent& data, const ObjInfo& info) {
		pup(puper, data.entity, info + ObjInfo{ "entity" });
	}

	void pup(Puper& puper, EntityActivateEvent& data, const ObjInfo& info) {
		pup(puper, data.entity, info + ObjInfo{ "entity" });
	}

	void pup(Puper& puper, EntityDeactivateEvent& data, const ObjInfo& info) {
		pup(puper, data.entity, info + ObjInfo{ "entity" });
	}

	void pup(Puper& puper, EntityCollisionEvent& data, const ObjInfo& info) {
		pup(puper, data.entity, info + ObjInfo{ "entity" });
		pup(puper, data.other, info + ObjInfo{ "other" });
		pup(puper, data.normal, info + ObjInfo{ "normal" });
		pup(puper, data.depth, info + ObjInfo{ "depth" });
	}


}