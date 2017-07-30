#pragma once

#include <graphics/batch.h>
#include <container.h>

struct Pipeline {
	int x, y, width, height; //view info
	const oak::vector<oak::graphics::Batch> *batches[2]; //list of batches
};