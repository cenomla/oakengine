#include "gl_renderer.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/material.h"

namespace oak::graphics {

	GLRenderer::GLRenderer() : vbo_{ GL_ARRAY_BUFFER }, ibo_{ GL_ELEMENT_ARRAY_BUFFER } {

	}

	void GLRenderer::init() {
		vao_.create();
		vao_.bind();
		
		vbo_.create();
		vbo_.bind();
		vao_.attributeDescription(
			32,
			{
				{ 0, 3, 0 },
				{ 1, 3, 12 },
				{ 2, 2, 24 }
			}
		);
		vbo_.unbind();

		ibo_.create();
		ibo_.bind();
		
		vao_.unbind();
	}

	void GLRenderer::render(const void *data, const Batch& batch) {
		vbo_.bind();
		vbo_.bufferData(batch.count * batch.meshDesc.stride, data, GL_STATIC_DRAW);
		vbo_.unbind();

		vao_.bind();

		glUseProgram(batch.material->shader->id);
		glBindTexture(GL_TEXTURE_2D, batch.material->texture->id);

		glDrawArrays(GL_TRIANGLES, 0, batch.count);
	}

}