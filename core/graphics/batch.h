#pragma once

#include "container.h"
#include "mesh.h"
#include "buffer_storage.h"

namespace oak::graphics {

	struct Material;

	struct Batch {
		AttributeLayout layout;
		const Material *material;
		const BufferStorage *storage;
		size_t offset, count;
	};

}