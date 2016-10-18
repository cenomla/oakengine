#pragma once

#include "script/luah.h"
#include "entity.h"

namespace oak::luah {

	void registerBindings(lua_State *L);

	void pushValue(lua_State *L, const Entity &entity);
	template<> Entity toValue(lua_State *L, int idx);

	void pushValue(lua_State *L, EntityManager &entityManager);
	template<> EntityManager& toValue(lua_State *L, int idx);


}