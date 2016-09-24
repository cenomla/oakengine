#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace oak::graphics {

	struct Vertex {
		glm::vec2 position;
		glm::vec3 color;
	};

	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

}