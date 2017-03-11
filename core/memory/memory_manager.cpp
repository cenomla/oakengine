#include "memory_manager.h"

#include <cmath>

#include "memory_literals.h"
#include "util/ptr_util.h"
#include "alloc.h"
#include "config.h"
#include "debug_vars.h"

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

	MemoryManager::MemoryManager()
	: memList_{ nullptr }, 
	allocator_{ allocate(config::GLOBAL_MEMORY_PAGE_SIZE), config::GLOBAL_MEMORY_PAGE_SIZE - headerSize }, 
	frameAllocator_{ allocate(config::FRAME_MEMORY_PAGE_SIZE), config::FRAME_MEMORY_PAGE_SIZE - headerSize } {
		INST = this;
		oak::proxyAllocator = { &allocator_ };
		oak::frameAllocator = { &frameAllocator_ };

		debugVars.allocatedMemory += config::GLOBAL_MEMORY_PAGE_SIZE + config::FRAME_MEMORY_PAGE_SIZE;
	}

	MemoryManager::~MemoryManager() {
		//free memory
		MemList *p = memList_;
		MemList *next = nullptr;
		while (p != nullptr) {
			next = p->next;
			free(p);
			p = next;
		}
		
		INST = nullptr;
	}

	void* MemoryManager::allocate(size_t size) {
		void *ptr = aligned_alloc(headerSize, size);
		MemList *l = static_cast<MemList*>(ptr);
		l->size = size;
		l->next = memList_;
		memList_ = l;

		return ptrutil::add(ptr, sizeof(MemList));
	}

	void MemoryManager::deallocate(void *ptr) {
		//search through linked list for ptr - sizeof(memList) then remove from list and deallocate
		ptr = ptrutil::subtract(ptr, sizeof(MemList));
		
		MemList *p = memList_;
		MemList *prev = nullptr;
		while (p != nullptr) {
			if (p == ptr) {
				if (prev != nullptr) {
					prev->next = p->next;
				} else {
					memList_ = p->next;
				}
				free(p);
				break;
			}
			prev = p;
			p = p->next;
		}
	}

}