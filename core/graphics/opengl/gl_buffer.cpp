#include "gl_buffer.h"

#include <glad/glad.h>

namespace oak::graphics {

	GLBuffer::GLBuffer(int32_t type) : type_{ type }, bid_{ 0 } {

	}

	GLBuffer::~GLBuffer() {
		destroy();
	}

	GLBuffer::GLBuffer(GLBuffer &&other) : type_{ other.type_ }, bid_{ other.bid_ } { 
		other.bid_ = 0;
	}

	void GLBuffer::operator=(GLBuffer &&other) {
		type_ = other.type_;
		bid_ = other.bid_;
		other.bid_ = 0;
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

	void *GLBuffer::map(int32_t flag) {
		bind();
		return glMapBuffer(type_, GL_WRITE_ONLY);
	}

	void GLBuffer::unmap() {
		bind();
		glUnmapBuffer(type_);
	}

	void GLBuffer::data(size_t size, const void *data, int32_t hint) {
		bind();
		glBufferData(type_, size, data, GL_STATIC_DRAW);
	}

	void GLBuffer::bindBufferBase(uint32_t index) const {
		if (type_ == GL_UNIFORM_BUFFER) {
			glBindBufferBase(type_, index, bid_);
		}
	}

}