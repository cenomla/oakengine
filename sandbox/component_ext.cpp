#include "component_ext.h"

namespace oak {

	void pup(Puper& puper, LightComponent& comp, const ObjInfo& info) {
		pup(puper, comp.size, info + ObjInfo{ "size" });
		pup(puper, comp.color, info + ObjInfo{ "color" });
	}

	void pup(Puper& puper, OccluderComponent& comp, const ObjInfo& info) {
		pup(puper, comp.height, info + ObjInfo{ "height" });
	}

}