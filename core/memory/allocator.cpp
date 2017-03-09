#include "allocator.h"

#include "memory_manager.h"

namespace oak::detail {

	void* allocate(size_t size, uint32_t pool, uint32_t align) {
		std::cout << "allocating " << MemoryManager::memoryString(size) << std::endl;
		return MemoryManager::inst().allocate(size, pool, align);
	}

	void deallocate(void *ptr, size_t size, uint32_t pool) {
		std::cout << "deallocating " << MemoryManager::memoryString(size) << std::endl;
		MemoryManager::inst().deallocate(ptr, size, pool);
	}

	size_t blockSize(uint32_t pool) {
		return MemoryManager::inst().getPoolBlockSize(pool);
	}
}