#pragma once

namespace oak::graphics {
	struct Texture;
	struct TextureInfo;
}

namespace oak::graphics::GLTexture {

	void bind(const Texture& texture, int slot);
	void unbind();

	Texture create(const char *path, int rcomp = 4);
	Texture create(const TextureInfo& info, void *data);
	
	void destroy(Texture& texture);

}