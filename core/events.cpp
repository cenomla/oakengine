#include "events.h"

#include "util/puper.h"

namespace oak::util {

	void pup(Puper &puper, KeyEvent &data, const ObjInfo &info) {
		pup(puper, data.key, ObjInfo{ "key" } + info);
		pup(puper, data.scancode, ObjInfo{ "scancode" } + info);
		pup(puper, data.action, ObjInfo{ "action" } + info);
		pup(puper, data.mods, ObjInfo{ "mods" } + info);
	}

	void pup(Puper &puper, ButtonEvent &data, const ObjInfo &info) {
		pup(puper, data.button, ObjInfo{ "button" } + info);
		pup(puper, data.action, ObjInfo{ "action" } + info);
		pup(puper, data.mods, ObjInfo{ "mods" } + info);
	}

	void pup(Puper &puper, MouseMoveEvent &data, const ObjInfo &info) {
		pup(puper, data.x, ObjInfo{ "x" } + info);
		pup(puper, data.y, ObjInfo{ "y" } + info);
	}

	void pup(Puper &puper, EntityActivateEvent &data, const ObjInfo &info) {
		pup(puper, data.entity, ObjInfo{ "entity" } + info);
	}

	void pup(Puper &puper, EntityDeactivateEvent &data, const ObjInfo &info) {
		pup(puper, data.entity, ObjInfo{ "entity" } + info);
	}

	void pup(Puper &puper, EntityCollisionEvent &data, const ObjInfo &info) {
		pup(puper, data.entity, ObjInfo{ "entity" } + info);
		pup(puper, data.other, ObjInfo{ "other" } + info);
		pup(puper, data.normal, ObjInfo{ "normal" } + info);
		pup(puper, data.depth, ObjInfo{ "depth" } + info);
	}


}