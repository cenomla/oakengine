#include "entity_id.h"

#include "pup.h"

namespace oak {

	void pup(Puper& puper, EntityId& data, const ObjInfo& info) {
		pup(puper, data.index, ObjInfo::make<uint32_t>(&info, "index"));
		pup(puper, data.generation, ObjInfo::make<uint32_t>(&info, "generation"));
	}

}