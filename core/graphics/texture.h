#pragma once

#include <cinttypes>
#include <glm/glm.hpp>

#include "container.h"
#include "resource.h"
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
		static const TypeInfo typeInfo;

		uint32_t id = 0;
		TextureInfo info;

		void destroy();
	};

	struct TextureAtlas {
		static const TypeInfo typeInfo;

		Texture texture;
		oak::vector<std::pair<oak::string, TextureRegion>> regions;

		void destroy();
	};

	void pup(Puper& puper, Texture& data, const ObjInfo& info);
	void pup(Puper& puper, TextureAtlas& data, const ObjInfo& info);

}
