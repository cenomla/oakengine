#pragma once

#include <cinttypes>
#include <glm/glm.hpp>

#include "container.h"
#include "api.h"

namespace oak::graphics {

	struct TextureRegion {
		glm::vec2 pos{ 0.0f };
		glm::vec2 extent{ 1.0f };
	};

	struct TextureInfo {
		TextureType type = TextureType::IMAGE_2D;

		TextureFormat format = TextureFormat::BYTE_RGBA;

		TextureFilter magFilter = TextureFilter::LINEAR;
		TextureFilter minFilter = TextureFilter::LINEAR;

		TextureWrap xWrap = TextureWrap::REPEAT;
		TextureWrap yWrap = TextureWrap::REPEAT;

		uint32_t width, height;
		int mipLevels = 1;
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