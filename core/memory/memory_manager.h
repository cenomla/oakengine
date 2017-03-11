#pragma once

#include <string>

#include "allocators.h"

namespace oak {

	class MemoryManager {
	public:
		inline static MemoryManager& inst() { return *INST; }
		static std::string memoryString(size_t byte);

		MemoryManager();
		~MemoryManager();

		void* allocate(size_t size);
		void deallocate(void *ptr);

		FreelistAllocator& getGlobalAllocator() { return allocator_; }
		LinearAllocator& getFrameAllocator() { return frameAllocator_; }
	private:
		static MemoryManager *INST;
		struct MemList {
			size_t size;
			MemList *next;
		};

		MemList *memList_;
		FreelistAllocator allocator_;
		LinearAllocator frameAllocator_;
	public:
		static constexpr size_t headerSize = sizeof(MemList);
	};

}