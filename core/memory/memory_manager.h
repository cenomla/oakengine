#pragma once

#include "oak_assert.h"
#include "memory_literals.h"
#include "alloc.h"

namespace oak {

	class MemoryManager {
	public:
		inline static MemoryManager& inst() { return *INST; }
		static std::string memoryString(size_t byte);

		MemoryManager();
		~MemoryManager();

		void* allocate(size_t size, uint32_t alignment = 8);
		void deallocate(void *ptr, size_t size);

		size_t getUsedMemory() const { return usedMemory_; }
		size_t getAllocatedMemory() const { return allocatedMemory_; }
	private:
		static MemoryManager *INST;
		FreelistAllocator allocator_;
		size_t usedMemory_;
		size_t allocatedMemory_;
	};

}