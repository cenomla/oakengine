#include "scene_events.h"

#include "pup.h"

namespace oak {

	const TypeInfo EntityCreateEvent::typeInfo = makeEventInfo<EntityCreateEvent>("entity_create");
	const TypeInfo EntityDestroyEvent::typeInfo = makeEventInfo<EntityDestroyEvent>("entity_destroy");
	const TypeInfo EntityActivateEvent::typeInfo = makeEventInfo<EntityActivateEvent>("entity_activate");
	const TypeInfo EntityDeactivateEvent::typeInfo = makeEventInfo<EntityDeactivateEvent>("entity_deactivate");

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
