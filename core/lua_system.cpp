#include "lua_system.h"

#include <lua/lua.hpp>

namespace oak {

	LuaSystem::LuaSystem(lua_State *L, const oak::string& name) : L_{ L }, name_{ name } {}

	void LuaSystem::init() {
		luah::getField(L_, LUA_REGISTRYINDEX, "_oak_systems_." + name_ + ".init");
		luah::call(L_, 0, 0);
	}

	void LuaSystem::terminate() {
		luah::getField(L_, LUA_REGISTRYINDEX, "_oak_systems_." + name_ + ".terminate");
		luah::call(L_, 0, 0);
	}

	void LuaSystem::run() {
		luah::getField(L_, LUA_REGISTRYINDEX, "_oak_systems_." + name_ + ".run");
		luah::call(L_, 0, 0);
	}

}
