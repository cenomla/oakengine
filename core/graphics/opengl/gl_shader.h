#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
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

		void setUniform(const std::string &name, const glm::mat4 &value) const;
		void setUniform(const std::string &name, const glm::vec3 &value) const;
		void setUniform(const std::string &name, const uint32_t value) const;
		void setUniform(const std::string &name, const float value) const;

		void bindBlockIndex(const std::string &name, GLuint binding) const;

		inline GLuint getId() const { return pid_; }

	private:
		GLuint pid_;
		std::unordered_map<std::string, GLuint> locations_;

		GLuint load(const std::string &path, GLenum type);
	};

}
