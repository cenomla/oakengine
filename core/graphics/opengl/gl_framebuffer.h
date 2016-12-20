#pragma once

#include <glad/glad.h>

namespace oak::graphics {

	class GLFramebuffer {
	public:
		GLFramebuffer();
		~GLFramebuffer();

		void create();
		void destroy();

		void bind();
		void unbind();

	private:
		GLuint fbo_;
		GLuint rbo_;
	};

}