#include "lua_manager.h"

#include "script/binding.h"
#include "engine.h"
#include "entity.h"

namespace oak {

	LuaManager::LuaManager(Engine &engine) : System{ engine, "lua_manager" } {

	}

	void LuaManager::init() {
		L_ = luah::createState();
		
		//create the global oak table
		lua_newtable(L_);
		//load the entity system script and add it to the oak table
		luah::loadScript(L_, "core/script/entity_system.lua");
		luah::pushValue(L_, engine_.getSystem<EntityManager>());
		lua_setfield(L_, -2, "manager");
		lua_setfield(L_, -2, "es");

		lua_setglobal(L_, "oak");
		
		luah::registerBindings(L_);
	}

	void LuaManager::destroy() {
		luah::destroyState(L_);
	}

}