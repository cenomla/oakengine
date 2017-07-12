#pragma once

#include <cstdint>
#include <cstddef>

namespace oak::graphics {
	struct Buffer;
	struct BufferInfo;
}

namespace oak::graphics::GLBuffer {

	void bind(const Buffer& buffer);
	void unbind(const Buffer& buffer);

	Buffer create(const BufferInfo& info);
	void destroy(Buffer& buffer);

	void* map(const Buffer& buffer);
	void unmap(const Buffer& buffer);

	void data(const Buffer& buffer, size_t size, const void *data);
	void data(const Buffer& buffer, size_t offset, size_t size, const void *data);

}