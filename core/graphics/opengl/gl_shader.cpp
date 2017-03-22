#include "gl_shader.h"

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

	void GLShader::create(const oak::string &vertPath, const oak::string &fragPath) {
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
			locations_.insert({ oak::string{ buffer },  glGetUniformLocation(pid_, buffer) });
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


	void GLShader::setUniform(const oak::string &name, const glm::mat4 &value) const {
		glUniformMatrix4fv(locations_.at(name), 1, GL_FALSE, &value[0][0]);
	}

	void GLShader::setUniform(const oak::string &name, const glm::vec3 &value) const {
		glUniform3fv(locations_.at(name), 1, &value[0]);
	}

	void GLShader::setUniform(const oak::string &name, const uint32_t value) const {
		glUniform1ui(locations_.at(name), value);
	}

	void GLShader::setUniform(const oak::string &name, const float value) const {
		glUniform1f(locations_.at(name), value);
	}

	void GLShader::bindBlockIndex(const oak::string &name, GLuint binding) const {
		GLuint index = glGetUniformBlockIndex(pid_, name.c_str());
		glUniformBlockBinding(pid_, index, binding);
	}

	GLuint GLShader::load(const oak::string &path, GLenum type) {	
		const oak::string code = util::readFileAsString(path);
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
		oak::vector<char> log(length + 1);
		glGetShaderInfoLog(shader, length, &length, log.data());
		if (length > 1) {
			log_print_out(log.data());
		}
		//log a failure
		if (result == GL_FALSE) {
			log_print_warn("failed to compile shader: %s", path.c_str());
		}

		return shader;
	}

}