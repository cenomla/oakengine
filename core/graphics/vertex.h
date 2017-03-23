#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace oak::graphics {
	
	struct Vertex {
		Vertex(const glm::vec2 &pos, const glm::vec2 &tex) : position{ pos }, texCoords{ tex } {}
		Vertex(float x, float y, float u, float v) : position{ x, y }, texCoords{ u, v } {}

		glm::vec2 position;
		glm::vec2 texCoords;
	};

	struct UniformBufferObject {
		glm::mat4 proj;
		glm::mat4 view;
		glm::mat4 model;
	};

}