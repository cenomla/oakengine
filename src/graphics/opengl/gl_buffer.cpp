#include "gl_buffer.h"

namespace oak::graphics {

	GLBuffer::GLBuffer(GLenum type) : type_{ type }, bid_{ 0 }, size_{ 0 } {

	}

	GLBuffer::~GLBuffer() {
		destroy();
	}

	void GLBuffer::bind() const {
		glBindBuffer(type_, bid_);
	}

	void GLBuffer::unbind() const {
		glBindBuffer(type_, 0);
	}

	void GLBuffer::create() {
		if (bid_ != 0) { return; }

		glGenBuffers(1, &bid_);
	}

	void GLBuffer::destroy() {
		if (bid_ != 0) {
			glDeleteBuffers(1, &bid_);
			bid_ = 0;
		}
	}

	void *GLBuffer::map(GLenum flag) const {
		return glMapBuffer(type_, flag);
	}

	void GLBuffer::unmap() const {
		glUnmapBuffer(type_);
	}

	void GLBuffer::bufferData(size_t size, const void *data, GLenum hint) const {
		glBufferData(type_, size, data, hint);
	}

	void GLBuffer::bufferData(size_t size, size_t offset, const void *data) const {
		glBufferSubData(type_, size, offset, data);
	}

	void GLBuffer::bindBufferBase(GLuint index) const {
		if (type_ == GL_UNIFORM_BUFFER) {
			glBindBufferBase(type_, index, bid_);
		}
	}

}