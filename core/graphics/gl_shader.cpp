#include "gl_shader.h"

#include <glad/glad.h>

#include <file_manager.h>
#include <log.h>

#include "shader.h"

namespace oak::graphics::GLShader {

	static GLuint load(const char *path, GLenum type);

	Shader create(const ShaderInfo& info) {

		//create the program
		uint32_t pid = glCreateProgram();
		oak::vector<GLuint> shaders;
		if (info.vertex) {
			shaders.push_back(load(info.vertex, GL_VERTEX_SHADER));
		}
		if (info.geometry) {
			shaders.push_back(load(info.geometry, GL_GEOMETRY_SHADER));
		}
		if (info.fragment) {
			shaders.push_back(load(info.fragment, GL_FRAGMENT_SHADER));
		}
		for (auto id : shaders) {
			glAttachShader(pid, id);
		}
		glLinkProgram(pid);
		glValidateProgram(pid);
		for (auto id : shaders) {
			glDeleteShader(id);
		}

		return { pid, info };

		/*
		//get all the uniform names
		char buffer[64];
		GLsizei length;
		GLint count;
		GLint size;
		GLenum type;
		int block;
		glGetProgramiv(pid, GL_ACTIVE_UNIFORMS, &count);

		for (GLuint i = 0; i < static_cast<GLuint>(count); i++) {
			//get the uniform information
			glGetActiveUniform(pid_, i, 64, &length, &size, &type, buffer);
			//get the block index
			block = glGetUniformBlockIndex(pid_, buffer);
			//log
			//log_print_out("uniform name: %s, size: %i, block index: %i", buffer, size, block);
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
		*/
	}

	void destroy(Shader& shader) {
		if (shader.id) {
			glDeleteProgram(shader.id);
			shader.id = 0;
		}
	}

	void bind(const Shader& shader) {
		glUseProgram(shader.id);;
	}

	void unbind() {
		glUseProgram(0);
	}


	void setUniform(const Shader& shader, const char *name, const glm::mat4& value) {
		glUniformMatrix4fv(glGetUniformLocation(shader.id, name), 1, GL_FALSE, &value[0][0]);
	}

	void setUniform(const Shader& shader, const char *name, const glm::ivec2& value) {
		glUniform2iv(glGetUniformLocation(shader.id, name), 1, &value[0]);
	}

	void setUniform(const Shader& shader, const char *name, const glm::vec2& value) {
		glUniform2fv(glGetUniformLocation(shader.id, name), 1, &value[0]);
	}

	void setUniform(const Shader& shader, const char *name, const glm::vec3& value) {
		glUniform3fv(glGetUniformLocation(shader.id, name), 1, &value[0]);
	}

	void setUniform(const Shader& shader, const char *name, const glm::vec4& value) {
		glUniform4fv(glGetUniformLocation(shader.id, name), 1, &value[0]);
	}

	void setUniform(const Shader& shader, const char *name, unsigned int value) {
		glUniform1ui(glGetUniformLocation(shader.id, name), value);
	}

	void setUniform(const Shader& shader, const char *name, int value) {
		glUniform1i(glGetUniformLocation(shader.id, name), value);
	}

	void setUniform(const Shader& shader, const char *name, float value) {
		glUniform1f(glGetUniformLocation(shader.id, name), value);
	}

	GLuint load(const char *path, GLenum type) {
		auto file = FileManager::inst().openFile(path);
		const oak::string code = file.read<oak::string>();
		FileManager::inst().closeFile(file);
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
			log_print_warn("failed to compile shader: %s", path);
		}

		return shader;
	}

}
