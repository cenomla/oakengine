#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>

namespace oak::graphics {

	class GLShader {
	public:
		GLShader();
		~GLShader();

		GLShader(GLShader &&other);
		void operator=(GLShader &&other);

		void create(const std::string &vertPath, const std::string &fragPath);
		void destroy();

		void bind() const;
		void unbind() const;

		void setMatrix4f(const std::string &name, const GLfloat *value) const;
		void setUniform1i(const std::string &name, const GLuint value) const;
		void setUniform1f(const std::string &name, const GLfloat value) const;

		void bindBlockIndex(const std::string &name, GLuint binding);

		inline GLuint getId() const { return pid_; }

	private:
		GLuint pid_;
		std::unordered_map<std::string, GLuint> locations_;

		GLuint load(const std::string &path, GLenum type);
	};

}
