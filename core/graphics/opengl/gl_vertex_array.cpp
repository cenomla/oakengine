#include "gl_vertex_array.h"

#include <glad/glad.h>

#include "graphics/attribute_layout.h"

namespace oak::graphics::GLVertexArray {

	uint32_t create() {
		uint32_t id;
		glGenVertexArrays(1, &id);
		return id;
	}

	void destroy(uint32_t id) {
		glDeleteVertexArrays(1, &id);
	}

	void bind(uint32_t id) {
		glBindVertexArray(id);
	}

	void unbind() {
		glBindVertexArray(0);
	}

	static const GLenum gltype[] = {
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_UNSIGNED_BYTE
	};

	static const GLenum normalized[] = {
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_TRUE
	};

	static const GLuint count[] = {
		3,
		2,
		3, 
		2, 
		4
	};

	void attributeDescription(const AttributeLayout *layout) {
		const size_t stride = layout->stride();
		size_t i = 0;
		size_t offset = 0;
		for (const auto& attrib : layout->attributes) {
			int type = static_cast<int>(attrib);
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, count[type], gltype[type], normalized[type], stride, reinterpret_cast<void*>(offset));
			switch(gltype[type]) {
				case GL_FLOAT: offset += count[type] * 4; break;
				case GL_UNSIGNED_BYTE: offset += count[type]; break;
			}
			i++;
		}
	}

}