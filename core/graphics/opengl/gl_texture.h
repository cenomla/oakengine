#pragma once

#include <glad/glad.h>

#include "memory/container.h"

namespace oak::graphics {

	class GLTexture {
	public:
		GLTexture(GLenum type, GLenum filter = GL_NEAREST);
		~GLTexture();

		void bind(GLenum slot) const;
		void unbind() const;

		void create(const oak::string &path);
		void create(int width, int height, void *data);
		void destroy();

		inline GLuint getId() const { return tex_; }
	protected:
		GLuint tex_;
		GLenum type_;
		GLenum filter_;
	};

}