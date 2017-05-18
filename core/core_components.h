#pragma once

#include <bitset>
#include "events.h"

namespace oak {

	class Puper;
	struct ObjInfo;

	struct EventComponent {
		std::bitset<config::MAX_EVENTS> filter;
	};

	struct PrefabComponent {
		size_t id;
	};

	void pup(Puper& puper, EventComponent& data, const ObjInfo& info);
	void pup(Puper& puper, PrefabComponent& data, const ObjInfo& info);

}