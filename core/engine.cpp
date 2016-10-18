#include "engine.h"

namespace oak {

	Engine *Engine::INST = nullptr;

	Engine::Engine() {
		INST = this;
	}

	Engine::~Engine() {
		INST = nullptr;
	}

	void Engine::init() {
		log::cout << "oak engine version: 0.1.0" << std::endl;
		taskManager_.init();
	}

	void Engine::destroy() {
		//wait for all tasks to finish
		taskManager_.destroy();

		//shutdown all systems
		for (auto &system : systems_) {
			system.second->destroy();
		}
	}

	void Engine::run() {
		taskManager_.run();
	}

	void Engine::removeSystem(const std::string &name) {
		for (auto it = std::begin(systems_); it != std::end(systems_); ++it) {
			if (it->second->getName() == name) {
				it->second->destroy();
				systems_.erase(it);
				break;
			}
		}
	}

	System* Engine::getSystem(const std::string &name) {
		for (auto it = std::begin(systems_); it != std::end(systems_); ++it) {
			if (it->second->getName() == name) {
				return it->second;
			}
		}
		log::cout << "system does not exist: " << name << std::endl;
		std::exit(-1);
	}

}