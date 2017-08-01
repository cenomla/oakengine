#pragma once

#include <cstddef>
#include <cinttypes>

#include "attribute_layout.h"
#include "buffer.h"

namespace oak::graphics {

	class BufferStorage {
	public:

		void create(const AttributeLayout& layout);
		void destroy();

		void instance(const AttributeLayout& layout, size_t offset);

		void bind() const;
		void unbind() const;

		void* map(int index);
		void unmap(int index);

		void data(int index, size_t size, const void *data);

	private:
		static uint32_t bound;
		uint32_t vao_;
		Buffer buffers_[2];
	};

}