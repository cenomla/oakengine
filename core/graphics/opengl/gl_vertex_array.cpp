#include "gl_vertex_array.h"

#include <glad/glad.h>

namespace oak::graphics {

	GLVertexArray::GLVertexArray() : vao_{ 0 } {}

	GLVertexArray::~GLVertexArray() {
		destroy();
	}

	void GLVertexArray::create() {
		glGenVertexArrays(1, &vao_);
	}

	void GLVertexArray::destroy() {
		if (vao_ != 0) {
			glDeleteVertexArrays(1, &vao_);
			vao_ = 0;
		}
	}

	void GLVertexArray::bind() const {
		glBindVertexArray(vao_);
	}

	void GLVertexArray::unbind() const {
		glBindVertexArray(0);
	}

	static GLenum gltype[] = {
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_UNSIGNED_BYTE
	};

	static GLenum normalized[] = {
		GL_FALSE,
		GL_FALSE,
		GL_FALSE,
		GL_TRUE
	};

	static GLuint count[] = {
		3, 
		3, 
		2, 
		3
	};

	void GLVertexArray::attributeDescription(const AttributeLayout& layout) {
		const size_t stride = layout.stride();
		size_t i = 0;
		size_t offset = 0;
		for (const auto& attrib : layout.attributes) {
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