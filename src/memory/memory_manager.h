#pragma once

#include <unordered_map>
#include <vector>

#include "oak_assert.h"
#include "memory_literals.h"
#include "block.h"
#include "alloc.h"

namespace oak {

	class MemoryManager {
	public:
		inline static MemoryManager& inst() { oak_assert(INST != nullptr); return *INST; }
		static std::string memoryString(size_t byte);
	private:
		static MemoryManager* INST;
	public:
		MemoryManager();
		~MemoryManager();

		void createPool(uint32_t id, size_t size);
		void destroyPool(uint32_t id);

		Block allocate(size_t size, uint32_t pool = 0);
		void deallocate(const Block &data, uint32_t pool = 0);

		size_t getAllocatedMemory(uint32_t id) const;
		size_t getUsedMemory(uint32_t id) const;

		size_t getAllocatedMemory() const;
		size_t getUsedMemory() const;
	private:
		struct MemoryPool {
			MemoryPool(size_t blockSize) : blockSize_{ blockSize }, allocatedMemory_{ 0 }, usedMemory_{ 0 } {}

			std::vector<FreelistAllocator> blocks_;

			size_t blockSize_;
			size_t allocatedMemory_;
			size_t usedMemory_;
		};

		
		size_t findInPool(const MemoryPool &pool, const void *ptr) const;

		std::unordered_map<uint32_t, MemoryPool> pools_;
	};

}