#include "core_components.h"

#include "type_info.h"
#include "pup.h"

namespace oak {

	const TypeInfo EventComponent::typeInfo = makeComponentInfo<EventComponent>("event");
	const TypeInfo PrefabComponent::typeInfo = makeComponentInfo<PrefabComponent>("prefab");

	void pup(Puper& puper, EventComponent& data, const ObjInfo& info) {
		oak::string f = data.filter.to_string().c_str();
		pup(puper, f, ObjInfo::make<decltype(data.filter)>(&info, "filter"));	
		data.filter = decltype(data.filter){ f };
	}

	void pup(Puper& puper, PrefabComponent& data, const ObjInfo& info) {
		pup(puper, data.id, ObjInfo::make<size_t>(&info, "id"));
	}

}
