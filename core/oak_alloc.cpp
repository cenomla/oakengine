#include "oak_alloc.h"

namespace oak {

	ProxyAllocator oalloc_proxy;
	FreelistAllocator oalloc_freelist{ &oalloc_proxy, 128_mb, 64 };
	LinearAllocator oalloc_frame{ &oalloc_freelist };

	OakAllocator<void> oak_allocator{ &oalloc_freelist };
	OakAllocator<void> frame_allocator{ &oalloc_frame };
	
}
