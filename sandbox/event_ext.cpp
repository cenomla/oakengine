#include "event_ext.h"

#include <util/puper.h>

namespace oak {

	void pup(Puper &puper, TileCollisionEvent &data, const ObjInfo &info) {
		pup(puper, data.entity, info + ObjInfo{ "entity" });
		pup(puper, data.tile, info + ObjInfo{ "tile" });
		pup(puper, data.normal, info + ObjInfo{ "normal" });
		pup(puper, data.depth, info + ObjInfo{ "depth" });
	}

}