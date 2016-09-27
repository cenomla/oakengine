#include "glshader.h"

#include "util/file_util.h"

namespace oak::graphics {

	GLShader::GLShader(GLenum shaderType) : shader_{ 0 }, type_{ shaderType } {

	}

	GLShader::~GLShader() {
		destroy();
	}

	void GLShader::load(const std::string &path) {
		if (shader_ != 0) { return; }
	
		const std::string code = util::readFileAsString(path);
		const char *cstr = code.c_str();

		shader_ = glCreateShader(type_);
		glShaderSource(shader_, 1, &cstr, NULL);
		glCompileShader(shader_);

		//get and print results of compiliation
		GLint result;
		glGetShaderiv(shader_, GL_COMPILE_STATUS, &result);
		//get log and print it
		GLint length;
		glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length + 1);
		glGetShaderInfoLog(shader_, length, &length, log.data());
		if (length > 1) {
			log::cout << log.data() << std::endl;
		}
		//exit of fail
		if (result == GL_FALSE) {
			log::cout << "failed to compile shader: " << path << std::endl;
		}
	}

	void GLShader::destroy() {
		if (shader_ != 0) {
			glDeleteShader(shader_);
			shader_ = 0;
		}
	}

	GLShader::operator GLuint() {
		return shader_;
	}


}