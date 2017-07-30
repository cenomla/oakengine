#pragma once

#include <glm/glm.hpp>

namespace oak::graphics {

	struct Camera {
		glm::mat4 proj{ 1.0f };
		glm::mat4 view{ 1.0f };

		void view3d(const glm::vec3& center, const glm::quat& rotation);
		void view2d(const glm::vec2& pos, float zoom);
	};

}