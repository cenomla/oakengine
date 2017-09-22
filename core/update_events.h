#pragma once

#include "event.h"

namespace oak {

	struct TickEvent {
		static const TypeInfo typeInfo;
	};

	struct SimulateEvent {
		static const TypeInfo typeInfo;
		float dt;
	};

	void pup(Puper& puper, TickEvent& data, const ObjInfo& info);
	void pup(Puper& puper, SimulateEvent& data, const ObjInfo& info);

}
