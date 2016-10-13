#pragma once

#include <glad/glad.h>
#include <string>

namespace oak::graphics {

	class GLTexture {
	public:
		GLTexture(GLenum type);
		~GLTexture();

		void bind(GLenum slot) const;
		void unbind() const;

		void create(const std::string &path);
		void destroy();
	private:
		GLuint tex_;
		GLenum type_;
	};

}