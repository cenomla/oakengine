#include "lua_manager.h"

#include "luah.h"

namespace oak {

	LuaManager* LuaManager::instance = nullptr;

	LuaManager::LuaManager() {
		oak_assert(instance == nullptr);
		instance = this;

		L_ = luah::createState();
	}

	LuaManager::~LuaManager() {
		oak_assert(instance != nullptr);
		instance = nullptr;

		luah::destroyState(L_);
	}

}
