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
		taskManager_.init(this);
	}

	void Engine::destroy() {

	}

	void Engine::start() {
		taskManager_.run();
	}

}