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
		GL_UNSIGNED_BYTE,
		GL_FLOAT,
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
		GL_TRUE,
		GL_FALSE,		
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
		4,
		16,
		3,
		2,
		3,
		2,
		4
	};

	static const GLuint divisor[] = {
		0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
	};

	static constexpr int glsize(GLenum type) {
		switch (type) {
			case GL_FLOAT: return 4;
			case GL_UNSIGNED_BYTE: return 1;
			default: return 0;
		}
	}

	void attribDescription(const AttributeLayout *layout) {
		const size_t stride = layout->stride();
		size_t i = 0;
		size_t offset = 0;
		for (const auto& attrib : layout->attributes) {
			int type = static_cast<int>(attrib);
			if (divisor[type] > 0) { i++; continue; }
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, count[type], gltype[type], normalized[type], stride, reinterpret_cast<void*>(offset));
			int s = glsize(gltype[type]);
			offset += s * count[type];
			i++;
		}
	}

	void instanceAttribDescription(const AttributeLayout *layout, size_t offset) {
		const size_t stride = layout->instance_stride();
		size_t i = 0;
		for (const auto& attrib : layout->attributes) {
			int type = static_cast<int>(attrib);
			if (divisor[type] < 1) { i++; continue; }
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, count[type], gltype[type], normalized[type], stride, reinterpret_cast<void*>(offset));
			glVertexAttribDivisor(i, divisor[type]);
			int s = glsize(gltype[type]);
			offset += s * count[type];
			i++;
		}
	}

}