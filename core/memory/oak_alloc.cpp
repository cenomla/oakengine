#include "oak_alloc.h"

#include "memory_manager.h"
#include "memory_literals.h"
#include "debug_vars.h"

namespace oak {

	oak_allocator<void> proxyAllocator;
	oak_allocator<void> frameAllocator{ nullptr, config::FRAME_MEMORY_PAGE_SIZE };
	
}