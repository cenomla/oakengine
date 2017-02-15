#include "view.h"

namespace oak {
	
	void pup(Puper& puper, View& view, const ObjInfo& info) {
		pup(puper, view.x, ObjInfo{ "x" } + info);
		pup(puper, view.y, ObjInfo{ "y" } + info);
		pup(puper, view.width, ObjInfo{ "width" } + info);
		pup(puper, view.height, ObjInfo{ "height" } + info);
	}


}