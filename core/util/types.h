#pragma once

#include <cstddef>
#include <cintypes>

namespace oak {

	struct MemBlock {
		void *next;
		size_t size;
	}

}
