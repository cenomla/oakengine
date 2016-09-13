#include "engine.h"

#include "system.h"

namespace oak {

	Engine *Engine::INST = nullptr;

	Engine::Engine() {
		INST = this;
	}

	Engine::~Engine() {
		INST = nullptr;
	}

	void Engine::init() {
		for (auto &system : systems_) {
			system.second->init();
		}

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
			systems_.insert({ name, system });
		}
	}

	void Engine::removeSystem(const std::string &name) {
		systems_.erase(name);
	}

}