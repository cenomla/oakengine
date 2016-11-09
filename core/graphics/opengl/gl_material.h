#pragma once

#include "gl_shader.h"
#include "gl_texture.h"

namespace oak::graphics {

	struct GLMaterial {
		size_t id;
		GLShader shader;
		GLTexture texture;

		inline void create(const std::string &shaderPath, const std::string &texturePath) {
			shader.create(shaderPath + "/opengl.vert", shaderPath + "/opengl.frag");
			texture.create(texturePath + ".png");
		}

		inline void bind() const {
			shader.bind();
			texture.bind(GL_TEXTURE0);
		}
	};

}