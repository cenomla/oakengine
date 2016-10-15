#include "system.h"

namespace oak {

	System::System(Engine &engine, const std::string &name) : engine_{ engine }, name_{ name } {

	}

	System::~System() {

	}

	void System::init() {

	}

	void System::destroy() {

	}
}