#include "resource_manager.h"

namespace oak {

	ResourceManager *ResourceManager::instance = nullptr;

	ResourceManager::ResourceManager() {
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
