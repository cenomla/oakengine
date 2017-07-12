#pragma once

#include <cstddef>
#include <cinttypes>

#include "attribute_layout.h"

namespace oak::graphics {

	class BufferStorage {
	public:

		virtual void create(const AttributeLayout *attribs) = 0;
		virtual void destroy() = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void* map(int index) = 0;
		virtual void unmap(int index) = 0;

		virtual void data(int index, size_t size, const void *data) = 0;
	};

}