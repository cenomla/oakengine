#pragma once

#include "mesh.h"

namespace oak::graphics {

	struct Material;

	struct Batch {
		AttributeLayout layout;
		const Material *material;
		size_t offset, count;
	};


}