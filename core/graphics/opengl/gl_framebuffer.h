#pragma once

#include <glad/glad.h>

#include "gl_texture.h"

namespace oak::graphics {

	class GLFramebuffer {
	public:
		GLFramebuffer();
		~GLFramebuffer();

		void create(const GLTexture &texture, int width, int height);
		void destroy();

		void bind() const;
		void unbind() const;

	private:
		GLuint fbo_;
		GLuint rbo_;
	};

}