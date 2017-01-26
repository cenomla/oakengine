#include "events.h"

#include <util/puper.h>

namespace oak {

	void pup(Puper &puper, TileCollisionEvent &data, const ObjInfo &info) {
		pup(puper, data.entity, ObjInfo{ "entity" } + info);
		pup(puper, data.tile, ObjInfo{ "tile" } + info);
		pup(puper, data.normal, ObjInfo{ "normal" } + info);
		pup(puper, data.depth, ObjInfo{ "depth" } + info);
	}

}