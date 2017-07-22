#include "system_manager.h"

namespace oak {

	SystemManager *SystemManager::instance = nullptr;

	SystemManager::SystemManager() {
		oak_assert(instance == nullptr);
		instance = this;
	}

	SystemManager::~SystemManager() {
		instance = nullptr;
		clear();
	}

	void SystemManager::clear() {
		//terminate systems
		for (auto system : systems_) {
			if (system != nullptr) {
				system->terminate();
			}
		}
		systems_.clear();
		names_.clear();
	}

}