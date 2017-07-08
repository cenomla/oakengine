#include "gl_shader.h"

#include "util/file_util.h"
#include "log.h"

namespace oak::graphics {
		
	GLShader::GLShader() : pid_{ 0 } {}

	GLShader::~GLShader() {
		destroy();
	}

	GLShader::GLShader(GLShader&& other) : pid_{ other.pid_ }, locations_{ std::move(other.locations_) } { 
		other.pid_ = 0;
	}

	void GLShader::operator=(GLShader&& other) {
		pid_ = other.pid_;
		locations_ = std::move(other.locations_);
		other.pid_ = 0;
	}

	void GLShader::create(const oak::string& vertPath, const oak::string& fragPath) {
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
		GLsizei length;
		GLint count;
		GLint size;
		GLenum type;
		int block;
		glGetProgramiv(pid_, GL_ACTIVE_UNIFORMS, &count);

		for (GLuint i = 0; i < static_cast<GLuint>(count); i++) {
			//get the uniform information
			glGetActiveUniform(pid_, i, 64, &length, &size, &type, buffer);
			//get the block index
			block = glGetUniformBlockIndex(pid_, buffer);
			//log
			log_print_out("uniform name: %s, size: %i, block index: %i", buffer, size, block);
			//only cache the locations of non block uniforms
			if (block != -1) { continue; }
			//make sure to handle all indices of an array
			for (int a = 0; a < size; a++) {
				if (size > 1) {
					sprintf(buffer + length - 3, "[%i]", a);
				}
				locations_.insert({ oak::string{ buffer },  glGetUniformLocation(pid_, buffer) });
			}
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


	void GLShader::setUniform(const oak::string& name, const glm::mat4& value) const {
		glUniformMatrix4fv(locations_.at(name), 1, GL_FALSE, &value[0][0]);
	}

	void GLShader::setUniform(const oak::string& name, const glm::vec3& value) const {
		glUniform3fv(locations_.at(name), 1, &value[0]);
	}

	void GLShader::setUniform(const oak::string& name, uint32_t value) const {
		glUniform1ui(locations_.at(name), value);
	}

	void GLShader::setUniform(const oak::string& name, int value) const {
		glUniform1i(locations_.at(name), value);
	}

	void GLShader::setUniform(const oak::string& name, float value) const {
		glUniform1f(locations_.at(name), value);
	}

	void GLShader::bindBlockIndex(const oak::string& name, GLuint binding) const {
		GLuint index = glGetUniformBlockIndex(pid_, name.c_str());
		glUniformBlockBinding(pid_, index, binding);
	}

	GLuint GLShader::load(const oak::string& path, GLenum type) {	
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