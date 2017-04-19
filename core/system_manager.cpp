#include "system_manager.h"

namespace oak {

	SystemManager *SystemManager::instance = nullptr;

	SystemManager::SystemManager() {
		instance = this;
	}

	SystemManager::~SystemManager() {
		instance = nullptr;
	}

}