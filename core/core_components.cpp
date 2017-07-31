#include "core_components.h"

#include "pup.h"

namespace oak {

	void pup(Puper& puper, EventComponent& data, const ObjInfo& info) {
		pup(puper, data.filter, ObjInfo::make<decltype(data.filter)>(&info, "filter"));	
	}

	void pup(Puper& puper, PrefabComponent& data, const ObjInfo& info) {
		pup(puper, data.id, ObjInfo::make<size_t>(&info, "id"));
	}

}