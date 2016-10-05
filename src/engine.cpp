#include "engine.h"

#include "system.h"
#include "log.h"

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
		taskManager_.init(this);
	}

	void Engine::destroy() {
		//wait for all tasks to finish
		taskManager_.destroy();


		//shutdown all systems
		for (auto &system : systems_) {
			system.second->destroy();
		}


	}

	void Engine::start() {
		taskManager_.run();
	}

	void Engine::addSystem(const std::string &name, System *system) {
		auto it = systems_.find(name);
		if (it == std::end(systems_)) {
			system->init();
			systems_.insert({ name, system });
		}
	}

	void Engine::removeSystem(const std::string &name) {
		auto it = systems_.find(name);
		if (it != std::end(systems_)) {
			it->second->destroy();
			systems_.erase(name);
		}
	}

	System* Engine::getSystem(const std::string &name) {
		auto it = systems_.find(name);
		if (it != std::end(systems_)) {
			return it->second;
		} else {
			log::cout << "system does not exist: " << name << std::endl;
			return nullptr;
		}
	}

}