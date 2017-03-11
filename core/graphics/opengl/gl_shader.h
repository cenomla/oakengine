#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "container.h"

namespace oak::graphics {

	class GLShader {
	public:
		GLShader();
		~GLShader();

		GLShader(GLShader &&other);
		void operator=(GLShader &&other);

		void create(const oak::string &vertPath, const oak::string &fragPath);
		void destroy();

		void bind() const;
		void unbind() const;

		void setUniform(const oak::string &name, const glm::mat4 &value) const;
		void setUniform(const oak::string &name, const glm::vec3 &value) const;
		void setUniform(const oak::string &name, const uint32_t value) const;
		void setUniform(const oak::string &name, const float value) const;

		void bindBlockIndex(const oak::string &name, GLuint binding) const;

		inline GLuint getId() const { return pid_; }

	private:
		GLuint pid_;
		oak::unordered_map<oak::string, GLuint> locations_;

		GLuint load(const oak::string &path, GLenum type);
	};

}
