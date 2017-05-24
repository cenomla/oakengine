#pragma once

#include <cinttypes>

namespace oak::graphics {

	struct TextureRegion {
		float dx, dy, dw, dh;
	};

	struct Texture {
		uint32_t id;
	};

}