#include "system_manager.h"

namespace oak {

	SystemManager *SystemManager::instance = nullptr;

	SystemManager::SystemManager() {
		oak_assert(instance == nullptr);
		instance = this;
	}

	SystemManager::~SystemManager() {
		instance = nullptr;
	}

}