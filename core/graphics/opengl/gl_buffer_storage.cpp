#include "gl_buffer_storage.h"

#include <glad/glad.h>

namespace oak::graphics {

	GLBufferStorage::GLBufferStorage() : vbo_{ GL_ARRAY_BUFFER }, ibo_{ GL_ELEMENT_ARRAY_BUFFER } {

	}

	void GLBufferStorage::create(const AttributeLayout *attribs) {
		//standard opengl vertex array setup with index buffer
		vao_.create();
		vao_.bind();

		vbo_.create();
		vbo_.bind();
		vao_.attributeDescription(attribs);
		vbo_.unbind();

		ibo_.create();
		ibo_.bind();
		
		vao_.unbind();
	}

	void GLBufferStorage::destroy() {
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