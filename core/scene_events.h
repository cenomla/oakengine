#pragma once

#include "entity_id.h"

namespace oak {

	class Puper;
	class ObjInfo;

	struct EntityCreateEvent {
		EntityId entity;
	};

	struct EntityDestroyEvent {
		EntityId entity;
	};

	struct EntityActivateEvent {
		EntityId entity;
	};

	struct EntityDeactivateEvent {
		EntityId entity;
	};

	void pup(Puper& puper, EntityCreateEvent& data, const ObjInfo& info);
	void pup(Puper& puper, EntityDestroyEvent& data, const ObjInfo& info);
	void pup(Puper& puper, EntityActivateEvent& data, const ObjInfo& info);
	void pup(Puper& puper, EntityDeactivateEvent& data, const ObjInfo& info);

}