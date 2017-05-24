#include "gl_renderer.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/material.h"

#include <glad/glad.h>

namespace oak::graphics {

	GLRenderer::GLRenderer() : vbo_{ GL_ARRAY_BUFFER }, ibo_{ GL_ELEMENT_ARRAY_BUFFER } {}

	void GLRenderer::init() {
		vao_.create();
		vbo_.create();
		vao_.bind();
		ibo_.create();
		ibo_.bind();
		vao_.unbind();
	}

	void GLRenderer::render(const Batch& batch) {
		
		vao_.bind();
		vbo_.bind();
		vao_.attributeDescription(batch.desc.stride, batch.desc.attributes);
		vbo_.unbind();

		glUseProgram(batch.material->shader->id);
		glBindTexture(GL_TEXTURE_2D, batch.material->texture->id);

		glDrawElements(GL_TRIANGLES, batch.count, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.offset * 4));

		vao_.unbind();
	}

}