#pragma once

#include "container.h"

#include "batch.h"

namespace oak::graphics {

	struct Pipeline {
		int x, y, width, height; //view info
		const oak::vector<Batch> *batches; //list of batches
	};

}