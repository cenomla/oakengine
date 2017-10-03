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

	void SystemManager::addSystem(System *system, size_t id) {
		systems_.insert({ id, system });
		system->init();
	}

	void SystemManager::removeSystem(size_t id) {
		const auto& it = systems_.find(id);
		if (it != std::end(systems_)) {
			systems_.erase(it);
		}	
	}

	System* SystemManager::getSystem(size_t id) {
		const auto& it = systems_.find(id);
		if (it != std::end(systems_)) {
			return it->second;
		} else {
			return nullptr;
		}
	}

	bool SystemManager::hasSystem(size_t id) {
		const auto& it = systems_.find(id);
		return it != std::end(systems_);
	}

	void SystemManager::clear() {
		//terminate systems
		for (auto system : systems_) {
			system.second->terminate();
		}
		systems_.clear();
	}

}
