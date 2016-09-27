#pragma once

#include <iostream>

#include <glad/glad.h>

namespace oak::graphics {

	class GLShader {
	public:
		GLShader(GLenum shaderType);
		~GLShader();

		void load(const std::string &path);
		void destroy();

		operator GLuint();
	private:
		GLuint shader_;
		GLenum type_;
		
	};

}