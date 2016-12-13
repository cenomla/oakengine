#include "binding.h"

#include <string>

#include "util/puper.h"
#include "script/lua_puper.h"
#include "resource_manager.h"
#include "components.h"
#include "engine.h"
#include "prefab.h"

namespace oak::luah {

	//void activate(entity)
	int c_entity_activate(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		entity.activate();

		return 0;
	}

	//void deactivate(entity)
	int c_entity_deactivate(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		entity.deactivate();

		return 0;
	}

	//uint index(entity)
	int c_entity_index(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		pushValue(L, entity.index());
	
		return 1;
	}

	//uint layer(entity)
	int c_entity_layer(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		pushValue(L, entity.layer());

		return 1;
	}

	//bool isActive(entity)
	int c_entity_isActive(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		pushValue(L, entity.isActive());

		return 1;
	}

	template<class T>
	int c_entity_getComponent(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		auto& c = entity.getComponent<T>();
		
		lua_newtable(L);
		LuaPuper puper{ L, 1 };
		pup(puper, c, {});

		return 1;
	}

	template<class T>
	int c_entity_setComponent(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		
		auto& c = entity.getComponent<T>();

		LuaPuper puper{ L, 2 };
		puper.setIo(util::PuperIo::IN);
		pup(puper, c, {});

		return 0;
	}

	//entity createEntity(layer, name)
	int c_entityManager_createEntity(lua_State *L) {
		uint8_t layer = static_cast<uint8_t>(toValue<uint32_t>(L, 1));
		std::string name = toValue<std::string>(L, 2);
		//clear stack
		lua_settop(L, 0);

		lua_newtable(L);
		getRegistry(L, name);
		if (isNil(L, -1)) {
			lua_pop(L, 1);//pop nil
			loadScript(L, "res/scripts/" + name + ".lua");//get the prefab on the stack
			lua_pushvalue(L, -1);//copy it
			lua_setfield(L, -2, "__index");//set its index to itself
			lua_setfield(L, LUA_REGISTRYINDEX, name.c_str());//assign it to the registry
			getRegistry(L, name);//get the prefabs table from the registry
		}
		getRegistry(L, "entity");
		lua_setmetatable(L, -2);
		lua_setmetatable(L, -2);


		const auto& prefab = Engine::inst().getSystem<ResourceManager>().require<Prefab>(name);
		Entity e = prefab.createInstance(layer);
		pushValue(L, e);

		lua_setfield(L, -2, "_id");
		

		return 1;
	}

	//void destroyEntity(entity)
	int c_entityManager_destroyEntity(lua_State *L) {
		Entity e = toValue<Entity>(L, 1);

		lua_settop(L, 0);

		e.destroy();
		return 0;
	}

	void registerBindings(lua_State *L) {
		addFunctionToMetatable(L, "entity", "activate", c_entity_activate);
		addFunctionToMetatable(L, "entity", "deactivate", c_entity_deactivate);
		addFunctionToMetatable(L, "entity", "index", c_entity_index);
		addFunctionToMetatable(L, "entity", "layer", c_entity_layer);
		addFunctionToMetatable(L, "entity", "isActive", c_entity_isActive);
		addFunctionToMetatable(L, "entity", "getTransform", c_entity_getComponent<TransformComponent>);
		addFunctionToMetatable(L, "entity", "setTransform", c_entity_setComponent<TransformComponent>);
		addFunctionToMetatable(L, "entity", "getPhysicsBody2d", c_entity_getComponent<PhysicsBody2dComponent>);
		addFunctionToMetatable(L, "entity", "setPhysicsBody2d", c_entity_setComponent<PhysicsBody2dComponent>);

		addFunctionToMetatable(L, "entity_manager", "createEntity", c_entityManager_createEntity);
		addFunctionToMetatable(L, "entity_manager", "destroyEntity", c_entityManager_destroyEntity);
	}

	void pushValue(lua_State *L, const Entity& e) {
		Entity *ptr = static_cast<Entity*>(lua_newuserdata(L, sizeof(Entity)));
		new (ptr) Entity{ e };
	}
	
	template<> Entity toValue(lua_State *L, int idx) {
		lua_getfield(L, idx, "_id");
		Entity *e = static_cast<Entity*>(lua_touserdata(L, -1));
		lua_pop(L, 1);
		return *e;
	}

	void pushValue(lua_State *L, EntityManager &entityManager) {
		pushInstance(L, entityManager);
		setMetatable(L, "entity_manager");
	}

	template<> EntityManager& toValue(lua_State *L, int idx) {
		return toInstance<EntityManager>(L, idx);
	}

}