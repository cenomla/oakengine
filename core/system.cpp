#include "system.h"

namespace oak {

	System::System(Engine &engine, const oak::string &name) : engine_{ engine }, name_{ name } {

	}

	System::~System() {

	}

	void System::init() {

	}

	void System::destroy() {

	}
}