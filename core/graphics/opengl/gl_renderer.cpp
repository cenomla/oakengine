#include "gl_renderer.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/material.h"
#include "gl_buffer.h"

namespace oak::graphics {

	void GLRenderer::init() {
		vao_.create();
		vao_.bind();		
		vao_.unbind();
	}

	void GLRenderer::render(BufferStorage *storage, const Batch& batch) {
		GLBuffer *buffer = static_cast<GLBuffer*>(storage);
		
		vao_.bind();
		buffer->bind();
		vao_.attributeDescription(
			32,
			{
				{ 0, 3, 0 },
				{ 1, 3, 12 },
				{ 2, 2, 24 }
			}
		);
		buffer->unbind();

		glUseProgram(batch.material->shader->id);
		glBindTexture(GL_TEXTURE_2D, batch.material->texture->id);

		glDrawArrays(GL_TRIANGLES, batch.offset, batch.count);

		vao_.unbind();
	}

}