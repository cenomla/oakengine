#pragma once

#include <cinttypes>
#include <glm/glm.hpp>

#include <container.h>

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

	oak::vector<Mesh> loadModel(const oak::string& path);

}