#include "lua_manager.h"



namespace oak {

	LuaManager::LuaManager(Engine &engine, const std::string &path) : System{ engine, "lua" }, path_{ path } {

	}

	void LuaManager::init() {
		L_ = luah::createState();
		//load the main script

		luah::loadScript(L_, path_);
	}

	void LuaManager::destroy() {
		luah::destroyState(L_);
	}

}