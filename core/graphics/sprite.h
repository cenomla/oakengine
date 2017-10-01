#pragma once

#include "math.h"
#include "texture.h"
#include "resource.h"

namespace oak::graphics {

	struct Sprite {
		static const TypeInfo typeInfo;

		struct Vertex {
			Vec2 position;
			Vec2 uv;
		};
		
		float centerX, centerY, width, height;
		TextureRegion region;
	};

	void pup(Puper& puper, Sprite& data, const ObjInfo& info);

}
