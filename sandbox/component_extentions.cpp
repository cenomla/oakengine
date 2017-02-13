#include "component_extentions.h"

namespace oak {

	void pup(Puper &puper, LightComponent &comp, const ObjInfo &info) {
		pup(puper, comp.size, ObjInfo{ "size" } + info);
		pup(puper, comp.color, ObjInfo{ "color" } + info);
	}

	void pup(Puper &puper, OccluderComponent &comp, const ObjInfo &info) {
		pup(puper, comp.height, ObjInfo{ "height" } + info);
	}

}