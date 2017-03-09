#include "memory_manager.h"

#include <cmath>

#include "log.h"

namespace oak {

	MemoryManager* MemoryManager::INST = nullptr;

	std::string MemoryManager::memoryString(size_t bytes) {
		double mem = static_cast<double>(bytes);
		std::string abr = "B";
		if (bytes > 1_tb) {
			mem /= 1e12f;
			abr = "TB";
		} else if (bytes > 1_gb) {
			mem /= 1e9f;
			abr = "GB";
		} else if (bytes > 1_mb) {
			mem /= 1e6f;
			abr = "MB";
		} else if (bytes > 1_kb) {
			mem /= 1e3f;
			abr = "KB";
		} 
		return std::to_string(static_cast<size_t>(std::round(mem))) + abr;
	}

	MemoryManager::MemoryManager() : allocator_{ malloc(4_mb), 4_mb }, usedMemory_{ 0 }, allocatedMemory_{ 4_mb } {
		INST = this;
	}

	MemoryManager::~MemoryManager() {

		log::cout << "total memory allocated: " << memoryString(allocatedMemory_) << std::endl;
		log::cout << "memory still in use: " << memoryString(usedMemory_) << std::endl;
		
		INST = nullptr;
	}

	void* MemoryManager::allocate(size_t size, uint32_t alignment) {
		usedMemory_ += size;
		void *ptr = allocator_.allocate(size, alignment);
		if (ptr == nullptr) {
			allocator_.append(malloc(4_mb), 4_mb);
			allocatedMemory_ += 4_mb;
			ptr = allocator_.allocate(size, alignment);
		}
		return ptr;
	}

	void MemoryManager::deallocate(void *ptr, size_t size) {
		allocator_.deallocate(ptr, size);
		usedMemory_ -= size;
	}

}