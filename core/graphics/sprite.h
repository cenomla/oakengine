#pragma once

#include <glm/glm.hpp>

#include "texture.h"
#include "resource.h"

namespace oak::graphics {

	struct Sprite {
		static const TypeInfo typeInfo;

		struct Vertex {
			glm::vec2 position;
			glm::vec2 uv;
		};
		
		float centerX, centerY, width, height;
		TextureRegion region;
	};

	void pup(Puper& puper, Sprite& data, const ObjInfo& info);

}
