#pragma once

#include <cinttypes>

namespace oak::graphics {

	enum class TextureFormat {
		BYTE_RGBA,
		FLOAT_RGB
	};

	struct TextureRegion {
		float dx, dy, dw, dh;
	};

	struct Texture {
		uint32_t id;
	};

}