#pragma once

#include <cstddef>
#include <cinttypes>
#include <glm/glm.hpp>

#include "texture.h"
#include "container.h"

namespace oak::graphics {

	struct Mesh {
		struct Vertex {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
		};

		oak::vector<Vertex> vertices;
		oak::vector<uint32_t> indices;
	};

}