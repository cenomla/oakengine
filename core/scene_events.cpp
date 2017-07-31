#include "scene_events.h"

#include "pup.h"

namespace oak {

	void pup(Puper& puper, EntityCreateEvent& data, const ObjInfo& info) {
		pup(puper, data.entity, ObjInfo::make<EntityId>(&info, "entity"));
	}

	void pup(Puper& puper, EntityDestroyEvent& data, const ObjInfo& info) {
		pup(puper, data.entity, ObjInfo::make<EntityId>(&info, "entity"));
	}

	void pup(Puper& puper, EntityActivateEvent& data, const ObjInfo& info) {
		pup(puper, data.entity, ObjInfo::make<EntityId>(&info, "entity"));
	}

	void pup(Puper& puper, EntityDeactivateEvent& data, const ObjInfo& info) {
		pup(puper, data.entity, ObjInfo::make<EntityId>(&info, "entity"));
	}

}