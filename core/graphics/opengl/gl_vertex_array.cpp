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

	static GLenum typeMap[] = {
		GL_FLOAT,
		GL_INT,
		GL_UNSIGNED_INT,
		GL_BYTE,
		GL_UNSIGNED_BYTE
	};

	void GLVertexArray::attributeDescription(size_t stride, const oak::vector<AttributeDescriptor>& attribs) {
		for (const auto& attrib : attribs) {
			glEnableVertexAttribArray(attrib.location);
			glVertexAttribPointer(attrib.location, attrib.count, typeMap[attrib.type], GL_FALSE, stride, reinterpret_cast<void*>(attrib.offset));
		}
	}

}