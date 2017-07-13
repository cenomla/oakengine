#pragma once

#include <glm/glm.hpp>

namespace oak::graphics {

	class Renderable {
	public:
		virtual void draw(void *buffer, void *ibuffer, const glm::mat4& transform, size_t startIndex);
	};

}