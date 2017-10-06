#include "stream.h"

#include "oak_alloc.h"

namespace oak {

	template<> void Stream::write(String str) {
		write<size_t>(str.length);
		buffer->write(str.length, str.data);
	}

	template<> String Stream::read() {
		size_t size = read<size_t>();
		Array<char> mem{ &oalloc_freelist };
		mem.init(size);
		buffer->read(size, mem.data);
		return String{ mem.data, size };
	}

}
