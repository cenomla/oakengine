#pragma once

#include <bitset>
#include "event.h"
#include "component.h"

namespace oak {

	struct EventComponent {
		static const TypeInfo typeInfo;
		std::bitset<config::MAX_EVENTS> filter;
	};

	struct PrefabComponent {
		static const TypeInfo typeInfo;
		size_t id;
	};

	void pup(Puper& puper, EventComponent& data, const ObjInfo& info);
	void pup(Puper& puper, PrefabComponent& data, const ObjInfo& info);

}
