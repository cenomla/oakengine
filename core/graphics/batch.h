#pragma once

#include "mesh.h"

namespace oak::graphics {

	struct Material;

	struct Batch {
		MeshDescriptor meshDesc;
		const Material *material;
		size_t offset, count;
	};


}