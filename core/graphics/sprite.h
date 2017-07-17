#pragma once

#include <glm/glm.hpp>
#include "texture.h"

namespace oak::graphics {


	struct Sprite {
		struct Vertex {
			glm::vec2 position;
			glm::vec2 uv;
		};
		
		float centerX, centerY, width, height;
		TextureRegion region;
	}

}