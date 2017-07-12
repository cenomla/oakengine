#pragma once

#include <cinttypes>

#include "container.h"
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

		TextureWrap xWrap = TextureWrap::REPEAT;
		TextureWrap yWrap = TextureWrap::REPEAT;

		uint32_t width, height;
	};

	struct Texture {
		uint32_t id = 0;
		TextureInfo info;
	};

	struct TextureAtlas {
		Texture texture;
		oak::vector<std::pair<oak::string, TextureRegion>> regions;
	};

}