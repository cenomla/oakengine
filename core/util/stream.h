#pragma once

#include <cstddef>

namespace oak {

	struct Stream {
		virtual void open() = 0;
		virtual void close() = 0;
		virtual void write(void *source, size_t size) = 0;
		virtual void read(void *dest, size_t size) = 0;
	};

}