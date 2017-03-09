#pragma once

#include "oak_assert.h"
#include "container.h"
#include "memory_literals.h"
#include "block.h"
#include "alloc.h"

namespace oak {

	class MemoryManager {
	public:
		inline static MemoryManager& inst() { oak_assert(INST != nullptr); return *INST; }
		static std::string memoryString(size_t byte);

		MemoryManager();
		~MemoryManager();

		void createPool(uint32_t id, size_t size);
		void destroyPool(uint32_t id);

		void* allocate(size_t size, uint32_t pool = 0, uint32_t alignment = 8);
		void deallocate(void* ptr, size_t size, uint32_t pool = 0);

		size_t getAllocatedMemory(uint32_t id) const;
		size_t getUsedMemory(uint32_t id) const;

		size_t getAllocatedMemory() const;
		size_t getUsedMemory() const;

		size_t getPoolBlockSize(uint32_t id) const { return pools_.at(id).blockSize_; }
	private:
		static MemoryManager* INST;

		struct MemoryPool {
			MemoryPool() : blockSize_{ 0 }, allocatedMemory_{ 0 }, usedMemory_{ 0 } {}
			MemoryPool(size_t blockSize) : blockSize_{ blockSize }, allocatedMemory_{ 0 }, usedMemory_{ 0 } {}

			std::vector<FreelistAllocator> blocks_;

			size_t blockSize_;
			size_t allocatedMemory_;
			size_t usedMemory_;
		};
				
		size_t findInPool(const MemoryPool &pool, const void *ptr) const;

		std::vector<MemoryPool> pools_;
	};

}