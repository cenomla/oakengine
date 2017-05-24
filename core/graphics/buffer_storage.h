#pragma once

#include <cstddef>
#include <cinttypes>

namespace oak::graphics {

	class BufferStorage {
	public:

		virtual void* map(int32_t flags) = 0;
		virtual void unmap() = 0;

		virtual void data(size_t size, const void *data, int32_t flags) = 0;
	};

}