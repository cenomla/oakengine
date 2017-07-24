#pragma once

#include <glm/glm.hpp>
#include "container.h"

namespace oak {

	struct Mesh2d {
		struct Vertex {
			glm::vec2 position;
			glm::vec2 normal;
		};

		oak::vector<Vertex> vertices;
	};

}