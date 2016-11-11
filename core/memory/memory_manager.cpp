#include "memory_manager.h"

#include <string>
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

	MemoryManager::MemoryManager() {
		INST = this;
		pools_.emplace_back(4_mb);
	}

	MemoryManager::~MemoryManager() {
		INST = nullptr;

		log::cout << "total memory allocated: " << memoryString(getAllocatedMemory()) << std::endl;
		log::cout << "memory still in use: " << memoryString(getUsedMemory()) << std::endl;
		
		for (auto &pool : pools_) {
			for (auto &block : pool.blocks_) {
				free(block.getStart());
			}
			pool.blocks_.clear();
		}
		pools_.clear();
	}

	void MemoryManager::createPool(uint32_t id, size_t size) {
		oak_assert(id != 0 && size > 0);
		if (id >= pools_.size()) {
			pools_.resize(id + 1);
		}
		if (pools_.at(id).blockSize_ == 0) {
			pools_[id] = MemoryPool{ size };
		}
	}

	void MemoryManager::destroyPool(uint32_t id) {
		oak_assert(id != 0);
		auto &pool = pools_.at(id);
		for (auto &block : pool.blocks_) {
			free(block.getStart());
		}
		pool = {};
	}

	void* MemoryManager::allocate(size_t size, uint32_t id, uint32_t alignment) {
		oak_assert(pools_.at(id).blockSize_ > 0);

		void *ptr = nullptr;
		auto &pool = pools_.at(id);
		//find a pool that will fit this size of allocation
		for (auto &block : pool.blocks_) {
			ptr = block.allocate(size, alignment);
			if (ptr != nullptr) {
				pool.usedMemory_ += size;
				return ptr;
			}
		}
		//if no pool has enough space allocate a new pool
		pool.blocks_.emplace_back(FreelistAllocator{ aligned_alloc(alignment, pool.blockSize_), pool.blockSize_ });
		pool.allocatedMemory_ += pool.blockSize_;
		pool.usedMemory_ += size;
		return pool.blocks_.back().allocate(size, alignment);
	}

	void MemoryManager::deallocate(void *ptr, size_t size, uint32_t id) {
		oak_assert(pools_.at(id).usedMemory_ > 0);

		auto &pool = pools_.at(id);
		size_t index = findInPool(pool, ptr);
		pool.blocks_[index].deallocate(ptr, size);
		pool.usedMemory_ -= size;
	}

	size_t MemoryManager::getAllocatedMemory(uint32_t id) const {
		return pools_.at(id).allocatedMemory_;
	}

	size_t MemoryManager::getUsedMemory(uint32_t id) const {
		return pools_.at(id).usedMemory_;
	}

	size_t MemoryManager::getAllocatedMemory() const {
		size_t sum = 0;
		for (const auto &pool : pools_) {
			sum += pool.allocatedMemory_;
		}
		return sum;
	}

	size_t MemoryManager::getUsedMemory() const {
		size_t sum = 0;
		for (const auto &pool : pools_) {
			sum += pool.usedMemory_;
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