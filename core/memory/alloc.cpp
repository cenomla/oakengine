#include "alloc.h"

#include "memory_manager.h"
#include "memory_literals.h"
#include "debug_vars.h"

namespace oak {

	OakAllocator<void> proxyAllocator;
	OakAllocator<void> frameAllocator{ nullptr, config::FRAME_MEMORY_PAGE_SIZE };
	
}