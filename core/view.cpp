#include "view.h"

namespace oak {
	
	void pup(Puper& puper, View& view, const ObjInfo& info) {
		pup(puper, view.x, info + ObjInfo{ "x" });
		pup(puper, view.y, info + ObjInfo{ "y" });
		pup(puper, view.width, info + ObjInfo{ "width" });
		pup(puper, view.height, info + ObjInfo{ "height" });
	}


}