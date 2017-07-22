#include "resource_manager.h"

namespace oak {

	ResourceManager *ResourceManager::instance = nullptr;

	ResourceManager::ResourceManager() {
		oak_assert(instance == nullptr);
		instance = this;
	}

	ResourceManager::~ResourceManager() {
		instance = nullptr;
	}

}
