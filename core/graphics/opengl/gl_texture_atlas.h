#pragma once

#include <stb_image.h>

#include "gl_texture.h"
#include "container.h"
#include "graphics/texture_region.h"

namespace oak::graphics {

	class GLTextureAtlas : public GLTexture {
	public:
		GLTextureAtlas(GLenum type, GLenum filter = GL_NEAREST);
		~GLTextureAtlas();

		void addTexture(const oak::string &name);
		
		void bake(int width, int height);
		void clear();

		const TextureRegion& getTextureRegion(const oak::string &name) const;
	private:
		struct TextureImage {
			oak::string name;
			int width, height, comp;
			stbi_uc* data;
		};

		oak::vector<std::pair<TextureImage, TextureRegion>> regions_;
	};

}