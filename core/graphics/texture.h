#pragma once

#include <cinttypes>

#include "api.h"

namespace oak::graphics {

	struct TextureRegion {
		float dx, dy, dw, dh;
	};

	struct TextureInfo {
		TextureType type = TextureType::IMAGE_2D;

		TextureFormat format = TextureFormat::BYTE_RGBA;

		TextureFilter magFilter = TextureFilter::LINEAR;
		TextureFilter minFilter = TextureFilter::LINEAR;

		TextureWrap xWrap = TextureWrap::CLAMP_EDGE;
		TextureWrap yWrap = TextureWrap::CLAMP_EDGE;

		uint32_t width, height;
	};

	struct Texture {
		uint32_t id = 0;
		TextureInfo info;
	};

}