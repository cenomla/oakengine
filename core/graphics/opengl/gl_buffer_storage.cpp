#include "gl_buffer_storage.h"

#include <glad/glad.h>

namespace oak::graphics {

	GLBufferStorage::GLBufferStorage() : vbo_{ GL_ARRAY_BUFFER }, ibo_{ GL_ELEMENT_ARRAY_BUFFER } {

	}

	void GLBufferStorage::init() {
		//standard opengl vertex array setup with index buffer (excluding the attribute descriptor part)
		vao_.create();
		vao_.bind();
		vbo_.create();
		ibo_.create();
		ibo_.bind();
		vao_.unbind();
	}

	void GLBufferStorage::terminate() {
		//destroy opengl objects
		vbo_.destroy();
		ibo_.destroy();
		vao_.destroy();
	}

	void GLBufferStorage::bind() const {
		vao_.bind();
	}

	void GLBufferStorage::unbind() const {
		vao_.unbind();
	}

	void GLBufferStorage::attributeDescription(const AttributeLayout& attribs) const {
		vbo_.bind();
		vao_.attributeDescription(attribs);
		vbo_.unbind();
	}

	void* GLBufferStorage::map(int index, uint32_t flags) {
		if (index == 0) {
			return vbo_.map(flags);
		}
		if (index == 1) {
			return ibo_.map(flags);
		}
		return nullptr;
	}

	void GLBufferStorage::unmap(int index) {
		if (index == 0) {
			vbo_.unmap();
		}
		if (index == 1) {
			ibo_.unmap();
		}
	}

	void GLBufferStorage::data(int index, size_t size, const void *data, uint32_t flags) {
		if (index == 0) {
			vbo_.data(size, data, flags);
		}
		if (index == 1) {
			ibo_.data(size, data, flags);
		}
	}

}