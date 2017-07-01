#pragma once

#include <cstdint>
#include <cstddef>

namespace oak::graphics {

	class GLBuffer {
	public:
		GLBuffer(int32_t type);
		~GLBuffer();

		GLBuffer(GLBuffer &&other);
		void operator=(GLBuffer &&other);

		void bind() const;
		void unbind() const;

		void create();
		void destroy();

		void* map(uint32_t flags);
		void unmap();

		void data(size_t size, const void *data, uint32_t flags);
		void data(size_t size, size_t offset, const void *data);

		void bindBufferBase(uint32_t index) const;
	private:
		int32_t type_;
		uint32_t bid_;
	};

}