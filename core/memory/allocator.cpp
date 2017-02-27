#include "allocator.h"

#include "memory_manager.h"

namespace oak::detail {

	void* allocate(size_t size, uint32_t pool, uint32_t align) {
		std::cout << "allocating " << MemoryManager::memoryString(size) << std::endl;
		if (size >= 8) {
			return MemoryManager::inst().allocate(size, pool, align);
		} else {
			return new char[size];
		}
	}

	void deallocate(void *ptr, size_t size, uint32_t pool) {
		std::cout << "deallocating " << MemoryManager::memoryString(size) << std::endl;
		if (size >= 8) {
			MemoryManager::inst().deallocate(ptr, size, pool);
		} else {
			delete[] ptr;
		}
	}

	size_t blockSize(uint32_t pool) {
		return MemoryManager::inst().getPoolBlockSize(pool);
	}
}