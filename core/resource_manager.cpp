#include "resource_manager.h"

namespace oak {

	ResourceManager *ResourceManager::instance = nullptr;

	ResourceManager::ResourceManager() {
		oak_assert(instance == nullptr);
		instance = this;
	}

	ResourceManager::~ResourceManager() {
		instance = nullptr;

		using namespace detail;

		for (auto it : resourceHandles_) {
			if (it != nullptr) {
				it->~ResourceStorageBase();
				oak_allocator.deallocate(it, sizeof(ResourceStorage<BaseResource>));
			}
		}
		resourceHandles_.clear();
	}

}
