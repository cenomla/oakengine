#pragma once

#include <bitset>
#include "events.h"

namespace oak {

	class Puper;
	struct ObjInfo;

	struct EventComponent {
		std::bitset<config::MAX_EVENTS> filter;
	};

	void pup(Puper& puper, EventComponent& data, const ObjInfo& info);

}