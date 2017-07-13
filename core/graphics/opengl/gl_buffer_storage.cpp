#include "gl_buffer_storage.h"

#include <glad/glad.h>

#include "gl_vertex_array.h"
#include "gl_buffer.h"

namespace oak::graphics {

	void GLBufferStorage::create(const AttributeLayout *layout) {
		//set layout
		layout_ = layout;
		//standard opengl vertex array setup with index buffer
		vao_ = GLVertexArray::create();
		GLVertexArray::bind(vao_);

		BufferInfo info;
		info.type = BufferType::VERTEX;
		info.hint = BufferHint::STREAM;

		vbo_ = GLBuffer::create(info);
		GLBuffer::bind(vbo_);
		GLVertexArray::attribDescription(layout_);
		GLBuffer::unbind(vbo_);

		info.type = BufferType::ELEMENT;
		ibo_ = GLBuffer::create(info);
		GLBuffer::bind(ibo_);
		
		GLVertexArray::unbind();
	}

	void GLBufferStorage::instance(size_t offset) {
		GLBuffer::bind(vbo_);
		GLVertexArray::instanceAttribDescription(layout_, offset);
		GLBuffer::unbind(vbo_);
	}

	void GLBufferStorage::destroy() {
		//destroy opengl objects
		GLBuffer::destroy(vbo_);
		GLBuffer::destroy(ibo_);
		GLVertexArray::destroy(vao_);
	}

	void GLBufferStorage::bind() const {
		GLVertexArray::bind(vao_);
	}

	void GLBufferStorage::unbind() const {
		GLVertexArray::unbind();
	}

	void* GLBufferStorage::map(int index) {
		if (index == 0) {
			GLBuffer::bind(vbo_);
			return GLBuffer::map(vbo_);
		}
		if (index == 1) {
			GLBuffer::bind(ibo_);
			return GLBuffer::map(ibo_);
		}
		return nullptr;
	}

	void GLBufferStorage::unmap(int index) {
		if (index == 0) {
			GLBuffer::unmap(vbo_);
		}
		if (index == 1) {
			GLBuffer::unmap(ibo_);
		}
	}

	void GLBufferStorage::data(int index, size_t size, const void *data) {
		if (index == 0) {
			GLBuffer::bind(vbo_);
			GLBuffer::data(vbo_, size, data);
		}
		if (index == 1) {
			GLBuffer::bind(ibo_);
			GLBuffer::data(ibo_, size, data);
		}
	}

}