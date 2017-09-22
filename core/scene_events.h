#pragma once

#include "entity_id.h"
#include "event.h"

namespace oak {

	struct EntityCreateEvent {
		static const TypeInfo typeInfo;
		EntityId entity;
	};

	struct EntityDestroyEvent {
		static const TypeInfo typeInfo;
		EntityId entity;
	};

	struct EntityActivateEvent {
		static const TypeInfo typeInfo;
		EntityId entity;
	};

	struct EntityDeactivateEvent {
		static const TypeInfo typeInfo;
		EntityId entity;
	};

	void pup(Puper& puper, EntityCreateEvent& data, const ObjInfo& info);
	void pup(Puper& puper, EntityDestroyEvent& data, const ObjInfo& info);
	void pup(Puper& puper, EntityActivateEvent& data, const ObjInfo& info);
	void pup(Puper& puper, EntityDeactivateEvent& data, const ObjInfo& info);

}
