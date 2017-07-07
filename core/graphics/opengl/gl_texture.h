#pragma once

#include <glad/glad.h>

#include "graphics/texture.h"
#include "graphics/api.h"
#include "container.h"

namespace oak::graphics {

	class GLTexture {
	public:
		GLTexture(GLenum type, TextureFormat format, GLenum filter = GL_NEAREST);
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
		TextureFormat format_;
		GLenum filter_;
	};

}