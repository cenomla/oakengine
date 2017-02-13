#include "gl_vertex_array.h"

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

	void GLVertexArray::attributeDescription(BindingData &&binding, std::initializer_list<AttributeData> &&attribs) {
		for (const auto& attrib : attribs) {
			glEnableVertexAttribArray(attrib.location);
			glVertexAttribPointer(attrib.location, attrib.format, GL_FLOAT, GL_FALSE, binding.stride, reinterpret_cast<void*>(attrib.offset));
		}
	}

}