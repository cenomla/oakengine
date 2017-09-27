#pragma once

#include "component.h"

namespace oak {

	struct PrefabComponent {
		static const TypeInfo typeInfo;
		size_t id;
	};

	void pup(Puper& puper, PrefabComponent& data, const ObjInfo& info);

}
