#include "gl_renderer.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/material.h"

#include <glad/glad.h>

namespace oak::graphics {

	GLRenderer::GLRenderer() : vbo_{ GL_ARRAY_BUFFER } {}

	void GLRenderer::init() {
		vao_.create();
		vbo_.create();
	}

	void GLRenderer::render(const Batch& batch) {
		
		vao_.bind();
		vbo_.bind();
		vao_.attributeDescription(batch.desc.stride, batch.desc.attributes);
		vbo_.unbind();

		glUseProgram(batch.material->shader->id);
		glBindTexture(GL_TEXTURE_2D, batch.material->texture->id);

		glDrawArrays(GL_TRIANGLES, batch.offset, batch.count);

		vao_.unbind();
	}

}