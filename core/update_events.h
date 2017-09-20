#pragma once

#include "type_info.h"

namespace oak {

	class Puper;
	struct ObjInfo;

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
