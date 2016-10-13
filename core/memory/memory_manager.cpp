#include "memory_manager.h"

#include <string>
#include <cmath>

#include "log.h"

namespace oak {

	MemoryManager* MemoryManager::INST = nullptr;

	std::string MemoryManager::memoryString(size_t bytes) {
		double mem = static_cast<double>(bytes);
		std::string abr = "B";
		if (mem > 1000000000000.0) {
			mem /= 1000000000000.0;
			abr = "TB";
		} else if (mem > 1000000000.0) {
			mem /= 1000000000.0;
			abr = "GB";
		} else if (mem > 1000000.0) {
			mem /= 1000000.0;
			abr = "MB";
		} else if (mem > 1000.0) {
			mem /= 1000.0;
			abr = "KB";
		} 
		return std::to_string(static_cast<size_t>(std::round(mem))) + abr;
	}

	MemoryManager::MemoryManager() {
		INST = this;
		pools_.emplace(std::pair<uint32_t, MemoryPool>{ 0, MemoryPool{ 16_mb } });
	}

	MemoryManager::~MemoryManager() {
		INST = nullptr;

		log::cout << "total memory allocated: " << memoryString(getAllocatedMemory()) << std::endl;
		log::cout << "memory still in use: " << memoryString(getUsedMemory()) << std::endl;
		
		for (auto &pool : pools_) {
			for (auto &block : pool.second.blocks_) {
				free(block.getStart());
			}
			pool.second.blocks_.clear();
		}
		pools_.clear();
	}

	void MemoryManager::createPool(uint32_t id, size_t size) {
		oak_assert(id != 0);
		pools_.emplace(std::pair<uint32_t, MemoryPool>{ id, MemoryPool{ size } });
	}

	void MemoryManager::destroyPool(uint32_t id) {
		oak_assert(id != 0);
		auto &pool = pools_.at(id);
		for (auto &block : pool.blocks_) {
			free(block.getStart());
		}
		pool.blocks_.clear();
		pools_.erase(id);
	}

	Block MemoryManager::allocate(size_t size, uint32_t id) {
		oak_assert(pools_.find(id) != std::end(pools_));

		void *ptr = nullptr;
		auto &pool = pools_.at(id);
		//find a pool that will fit this size of allocation
		for (auto &block : pool.blocks_) {
			ptr = block.allocate(size);
			if (ptr != nullptr) {
				pool.usedMemory_ += size;
				return { ptr, size };
			}
		}
		//if no pool has enough space allocate a new pool
		pool.blocks_.emplace_back(FreelistAllocator{ malloc(pool.blockSize_), pool.blockSize_ });
		pool.allocatedMemory_ += pool.blockSize_;
		pool.usedMemory_ += size;
		return { pool.blocks_.back().allocate(size), size };
	}

	void MemoryManager::deallocate(const Block &data, uint32_t id) {
		oak_assert(pools_.find(id) != std::end(pools_));

		auto &pool = pools_.at(id);
		size_t index = findInPool(pool, data.ptr);
		pool.blocks_[index].deallocate(data.ptr, data.size);
		pool.usedMemory_ -= data.size;
	}

	size_t MemoryManager::getAllocatedMemory(uint32_t id) const {
		return pools_.at(id).allocatedMemory_;
	}

	size_t MemoryManager::getUsedMemory(uint32_t id) const {
		return pools_.at(id).usedMemory_;
	}

	size_t MemoryManager::getAllocatedMemory() const {
		size_t sum = 0;
		for (auto const &pool : pools_) {
			sum += pool.second.allocatedMemory_;
		}
		return sum;
	}

	size_t MemoryManager::getUsedMemory() const {
		size_t sum = 0;
		for (auto const &pool : pools_) {
			sum += pool.second.usedMemory_;
		}
		return sum;
	}

	size_t MemoryManager::findInPool(const MemoryPool &pool, const void *ptr) const {
		const uintptr_t p = reinterpret_cast<const uintptr_t>(ptr);

		for (size_t i = 0; i < pool.blocks_.size(); i++) {
			auto &block = pool.blocks_[i];
			uintptr_t start = reinterpret_cast<uintptr_t>(block.getStart());
			if (p >= start && p < start + block.getSize()) {
				return i;
			}
		}

		return -1;
	}

}