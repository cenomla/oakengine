#pragma once

#include "container.h"
#include "mesh.h"
#include "buffer_storage.h"

namespace oak::graphics {

	struct Material;

	struct Batch {
		const BufferStorage *storage = nullptr;
		const Material *material = nullptr;
		size_t offset = 0, count = 0;
		uint32_t layer = 0;
		int instances = -1;
	};

}