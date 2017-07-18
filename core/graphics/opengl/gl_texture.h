#pragma once

#include "container.h"

namespace oak::graphics {
	struct Texture;
	struct TextureInfo;
	struct TextureAtlas;
}

namespace oak::graphics::GLTexture {

	void bind(const Texture& texture, int slot);
	void unbind();

	Texture create(const char *path, const TextureInfo& info);
	Texture create(const TextureInfo& info, void *data);

	TextureAtlas createAtlas(const oak::vector<const char*>& paths, const TextureInfo& info);
	Texture createCubemap(const oak::vector<const char*>& paths, const TextureInfo& info);
	
	void destroy(Texture& texture);

}