#pragma once

#include <cstddef>
#include <cinttypes>

#include "attribute_layout.h"

namespace oak::graphics {

	class BufferStorage {
	public:

		virtual void init(const AttributeLayout *attribs) = 0;
		virtual void terminate() = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void* map(int index, uint32_t flags) = 0;
		virtual void unmap(int index) = 0;

		virtual void data(int index, size_t size, const void *data, uint32_t flags) = 0;
	};

}