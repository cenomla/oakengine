#include "lua_manager.h"



namespace oak {

	LuaManager::LuaManager(Engine &engine, const std::string &path) : System{ engine, "lua" }, path_{ path } {

	}

	void LuaManager::init() {
		L_ = luah::createState();

		//create the global oak table
		lua_newtable(L_);
		//load the entity system script and add it to the oak table
		luah::loadScript(L_, "core/script/entity_system.lua");
		lua_setfield(L_, -1, "es");
		lua_setglobal(L_, "oak");

		//load the main script
		luah::loadScript(L_, path_);
	}

	void LuaManager::destroy() {
		luah::destroyState(L_);
	}

}