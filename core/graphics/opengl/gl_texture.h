#pragma once

#include <glad/glad.h>
#include <string>

namespace oak::graphics {

	class GLTexture {
	public:
		GLTexture(GLenum type, GLenum filter = GL_NEAREST);
		~GLTexture();

		void bind(GLenum slot) const;
		void unbind() const;

		void create(const std::string &path);
		void destroy();
	protected:
		GLuint tex_;
		GLenum type_;
		GLenum filter_;
	};

}