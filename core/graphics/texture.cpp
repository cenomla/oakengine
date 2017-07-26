#include "texture.h"

#include "gl_api.h"

namespace oak::graphics {

	void Texture::destroy() {
		texture::destroy(*this);
	}

	void TextureAtlas::destroy() {
		texture::destroy(texture);
	}

}