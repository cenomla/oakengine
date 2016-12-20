#pragma once

#include <string>
#include <vector>
#include <stb_image.h>

#include "gl_texture.h"
#include "graphics/texture_region.h"

namespace oak::graphics {

	class GLTextureAtlas : public GLTexture {
	public:
		GLTextureAtlas(GLenum type, GLenum filter = GL_NEAREST);
		~GLTextureAtlas();

		void addTexture(const std::string &name);
		
		void bake(int width, int height);
		void clear();

		const TextureRegion& getTextureRegion(const std::string &name) const;
	private:
		struct TextureImage {
			std::string name;
			int width, height, comp;
			stbi_uc* data;
		};

		std::vector<std::pair<TextureImage, TextureRegion>> regions_;
	};

}