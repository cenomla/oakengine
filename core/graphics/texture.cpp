#include "texture.h"

#include "gl_api.h"

namespace oak::graphics {

	const TypeInfo Texture::typeInfo = makeResourceInfo<Texture>("texture");
	const TypeInfo TextureAtlas::typeInfo = makeResourceInfo<TextureAtlas>("atlas");
	
	void pup(Puper& puper, Texture& data, const ObjInfo& info) {}
	void pup(Puper& puper, TextureAtlas& data, const ObjInfo& info) {}

	void Texture::destroy() {
		texture::destroy(*this);
	}

	void TextureAtlas::destroy() {
		texture::destroy(texture);
	}

}
