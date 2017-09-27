#include "core_components.h"

namespace oak {

	const TypeInfo PrefabComponent::typeInfo = makeComponentInfo<PrefabComponent>("prefab");

	void pup(Puper& puper, PrefabComponent& data, const ObjInfo& info) {
		pup(puper, data.id, ObjInfo::make<size_t>(&info, "id"));
	}

}
