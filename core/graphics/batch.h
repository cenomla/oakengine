#pragma once

#include "container.h"
#include "mesh.h"
#include "buffer_storage.h"

namespace oak::graphics {

	struct Material;

	struct Batch {
		const BufferStorage *storage;
		const Material *material;
		size_t offset, count;
		uint32_t layer;
	};

}