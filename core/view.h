#pragma once

#include "pup.h"

namespace oak {

	struct View {
		int x = 0, y = 0, width = 0, height = 0;
	};

	void pup(Puper& puper, View& data, const ObjInfo& info);

}