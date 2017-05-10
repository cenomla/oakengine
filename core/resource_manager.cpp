#include "resource_manager.h"

namespace oak {

	ResourceManager *ResourceManager::instance = nullptr;

	ResourceManager::ResourceManager() {
		oak_assert(instance == nullptr);
		instance = this;
	}

	ResourceManager::~ResourceManager() {
		instance = nullptr;
		for (auto handle : resourceHandles_) {
			if (handle != nullptr) {
				handle->~ResourceListHandlerBase();
				oak_allocator.deallocate(handle, sizeof(ResourceListHandler<detail::BaseResource>));
			}
		}
	}

}
