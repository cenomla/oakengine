#pragma once

#include "pup.h"

namespace oak {

	struct View {
		float x = 0, y = 0, width = 0, height = 0;
	};

	void pup(Puper& puper, View& data, const ObjInfo& info);

}