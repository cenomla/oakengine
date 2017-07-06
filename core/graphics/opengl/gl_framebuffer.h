#pragma once

#include <glad/glad.h>

#include "container.h"
#include "gl_texture.h"

namespace oak::graphics {

	class GLFramebuffer {
	public:
		GLFramebuffer();
		~GLFramebuffer();

		void addTexture(const GLTexture &texture, int mipmap);

		void create(int width, int height);
		void destroy();

		void bind() const;
		void unbind() const;

	private:
		struct TexTargetInfo {
			const GLTexture *tex;
			int mipmap;	
		};

		GLuint fbo_;
		GLuint rbo_;
		oak::vector<TexTargetInfo> textures_;
	};

}