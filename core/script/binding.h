#pragma once

#include "script/luah.h"
#include "script/lua_puper.h"
#include "entity.h"
#include "engine.h"

namespace oak {
namespace luah {

	void registerBindings(lua_State *L);

	void pushValue(lua_State *L, const Entity &entity);
	template<> Entity toValue(lua_State *L, int idx);

}

	template<class T>
	int c_entity_addComponent(lua_State *L) {
		Entity entity = luah::toValue<Entity>(L, 1);

		T& comp = entity.addComponent<T>();

		LuaPuper puper{ L, 2 };
		puper.setIo(PuperIo::IN);
		pup(puper, comp, {});
		
		lua_settop(L, 0);

		return 0;
	}

	template<class T>
	int c_entity_removeComponent(lua_State *L) {
		Entity entity = luah::toValue<Entity>(L, 1);
		lua_settop(L, 0);

		entity.removeComponent<T>();
	
		return 0;
	}

	template<class T>
	int c_entity_getComponent(lua_State *L) {
		Entity entity = luah::toValue<Entity>(L, 1);
		lua_settop(L, 0);

		auto& c = entity.getComponent<T>();
		
		lua_newtable(L);
		LuaPuper puper{ L, 1 };
		pup(puper, c, {});

		return 1;
	}

	template<class T>
	int c_entity_setComponent(lua_State *L) {
		Entity entity = luah::toValue<Entity>(L, 1);
		
		auto& c = entity.getComponent<T>();

		LuaPuper puper{ L, 2 };
		puper.setIo(PuperIo::IN);
		pup(puper, c, {});

		return 0;
	}

	template<class T>
	int c_entity_hasComponent(lua_State *L) {
		Entity entity = luah::toValue<Entity>(L, 1);
		lua_settop(L, 0);

		bool has = entity.hasComponent<T>();

		lua_pushboolean(L, has);

		return 1;
	}

	template<class T>
	int c_emitEvent(lua_State *L) {
		T evt;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, evt, {});

		lua_settop(L, 0);

		EventManager::inst().getQueue<T>().emit(evt);

		return 0;
	}


}