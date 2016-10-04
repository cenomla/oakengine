#include "glshader.h"

#include <vector>

#include "util/file_util.h"
#include "log.h"

namespace oak::graphics {
		
	GLShader::GLShader() : pid_{ 0 } {}

	GLShader::~GLShader() {
		destroy();
	}

	GLShader::GLShader(GLShader &&other) : pid_{ other.pid_ }, locations_{ std::move(other.locations_) } { 
		other.pid_ = 0;
	}

	void GLShader::operator=(GLShader &&other) {
		pid_ = other.pid_;
		locations_ = std::move(other.locations_);
		other.pid_ = 0;
	}

	void GLShader::create(const std::string &vertPath, const std::string &fragPath) {
		if (pid_ != 0) { return; }

		//create the program
		GLuint vert = load(vertPath, GL_VERTEX_SHADER);
		GLuint frag = load(fragPath, GL_FRAGMENT_SHADER);
		pid_ = glCreateProgram();
		glAttachShader(pid_, vert);
		glAttachShader(pid_, frag);
		glLinkProgram(pid_);
		glValidateProgram(pid_);
		glDeleteShader(vert);
		glDeleteShader(frag);

		//get all the uniform names
		char buffer[64];
		GLint count;
		glGetProgramiv(pid_, GL_ACTIVE_UNIFORMS, &count);

		for (GLuint i = 0; i < static_cast<GLuint>(count); i++) {
			//get the block index
			int blockIndex;
			glGetActiveUniformsiv(pid_, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIndex);
			//if the block index is -1 (the default block) then get the name and cache the location
			if (blockIndex != -1) { continue; }
			glGetActiveUniformName(pid_, i, 64, NULL, buffer);
			locations_.insert({ std::string{ buffer },  glGetUniformLocation(pid_, buffer) });
		}
	}

	void GLShader::destroy() {
		if (pid_ != 0) {
			glDeleteProgram(pid_);
			pid_ = 0;
		}
	}

	void GLShader::bind() const {
		glUseProgram(pid_);
	}

	void GLShader::unbind() const {
		glUseProgram(0);
	}


	void GLShader::setMatrix4f(const std::string &name, const GLfloat *value) const {
		glUniformMatrix4fv(locations_.at(name.c_str()), 1, GL_FALSE, value);
	}

	void GLShader::setUniform1i(const std::string &name, const GLuint value) const {
		glUniform1i(locations_.at(name.c_str()), value);
	}

	void GLShader::setUniform1f(const std::string &name, const GLfloat value) const {
		glUniform1f(locations_.at(name.c_str()), value);
	}

	void GLShader::bindBlockIndex(const std::string &name, GLuint binding) {
		GLuint index = glGetUniformBlockIndex(pid_, name.c_str());
		glUniformBlockBinding(pid_, index, binding);
	}

	GLuint GLShader::load(const std::string &path, GLenum type) {	
		const std::string code = util::readFileAsString(path);
		const char *cstr = code.c_str();

		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &cstr, NULL);
		glCompileShader(shader);

		//get and print results of compiliation
		GLint result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		//get log and print it
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length + 1);
		glGetShaderInfoLog(shader, length, &length, log.data());
		if (length > 1) {
			log::cout << log.data() << std::endl;
		}
		//log a failure
		if (result == GL_FALSE) {
			log::cout << "failed to compile shader: " << path << std::endl;
		}

		return shader;
	}

}