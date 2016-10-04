#pragma once

#include <glad/glad.h>

namespace oak::graphics {

	class GLBuffer {
	public:
		GLBuffer(GLenum type);
		~GLBuffer();

		void bind() const;
		void unbind() const;

		void create();
		void destroy();

		void* map(GLenum flag) const;
		void unmap() const;

		void bufferData(size_t size, const void *data, GLenum hint) const;
		void bufferData(size_t size, size_t offset, const void *data) const;

		void bindBufferBase(GLuint index) const;
	private:
		GLenum type_;
		GLuint bid_;
		size_t size_;
	};

}