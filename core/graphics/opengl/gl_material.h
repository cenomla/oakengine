#pragma once

#include <functional>

#include "gl_shader.h"
#include "gl_texture.h"

namespace oak::graphics {

	struct GLMaterial {
		size_t id;
		const GLShader *shader;
		const GLTexture *texture;

		inline void bind() const {
			shader->bind();
			texture->bind(GL_TEXTURE0);
		}
	};

}