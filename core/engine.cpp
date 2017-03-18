#include "engine.h"

#include <functional>

#include "events.h"

namespace oak {

	Engine *Engine::INST = nullptr;

	Engine::Engine() : taskManager_{ 1 }, isRunning_{ false } {
		INST = this;
	}

	Engine::~Engine() {
		INST = nullptr;
	}

	void Engine::init() {
		log::cout << "oak engine version: 0.1.0" << std::endl;
		taskManager_.init();
		eventManager_.add<QuitEvent>(std::ref(*this));
		isRunning_ = true;
	}

	void Engine::destroy() {
		//wait for all tasks to finish
		taskManager_.destroy();

		//shutdown all systems
		for (auto &system : systems_) {
			system.second->destroy();
		}
	}

	void Engine::removeSystem(const oak::string& name) {
		for (auto it = std::begin(systems_); it != std::end(systems_); ++it) {
			if (it->second->getName() == name) {
				it->second->destroy();
				systems_.erase(it);
				break;
			}
		}
	}

	System* Engine::getSystem(const oak::string& name) {
		for (auto it = std::begin(systems_); it != std::end(systems_); ++it) {
			if (it->second->getName() == name) {
				return it->second;
			}
		}
		log::cout << "system does not exist: " << name << std::endl;
		abort();
	}

	bool Engine::hasSystem(const oak::string& name) {
		for (auto it = std::begin(systems_); it != std::end(systems_); ++it) {
			if (it->second->getName() == name) {
				return true;
			}
		}
		return false;
	}

}