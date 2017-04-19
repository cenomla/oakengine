#include "lua_system.h"

namespace oak {

	void LuaManager::init() {
		L_ = luah::createState();
		
		//create the prefabs table in the registry
		lua_newtable(L_);
		lua_setfield(L_, LUA_REGISTRYINDEX, "_prefabs_");

		//create the global oak table
		lua_newtable(L_);
		//load the entity system script and add it to the oak table
		luah::loadScript(L_, "core/script/entity_system.lua");
		//create the manager table for accessing the binding withing the entity_system table
		lua_newtable(L_);
		luah::setMetatable(L_, "entity_manager");
		//assign the manager value to the entity_system table
		lua_setfield(L_, -2, "manager");
		//assign all that to the global oak table
		lua_setfield(L_, -2, "es");

		//set the metatable of the oak global table and assign it to the global scope
		luah::setMetatable(L_, "oak");
		lua_setglobal(L_, "oak");
		
		//luah::registerBindings(L_);
	}

	void LuaManager::terminate() {
		luah::destroyState(L_);
	}

}