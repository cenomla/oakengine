#include "binding.h"

#include <string>

#include "util/puper.h"
#include "script/lua_puper.h"
#include "resource_manager.h"
#include "components.h"
#include "engine.h"
#include "prefab.h"

namespace oak::luah {

	//void create_prefab(manager, name, table)
	int c_create_prefab(lua_State *L) {
		EntityManager &manager = toValue<EntityManager&>(L, 1);
		std::string name = toValue<std::string>(L, 2);
		lua_pushvalue(L, 3);
		auto keys = getKeys(L);

		Prefab &prefab = Engine::inst().getSystem<ResourceManager>().add<Prefab>(name, &manager);

		for (const auto &k : keys) {
			lua_getfield(L, -1, k.c_str());
			//check whether or not this component should be shared default no
			lua_getfield(L, -1, "shared");
			bool shared = false;
			if (!isNil(L, -1)) {
				shared = toValue<bool>(L, -1);
			}
			lua_pop(L, 1);

			//deserialize the component
			size_t tid = manager.getComponentId(k);
			void *comp = prefab.addComponent(shared, tid);

			LuaPuper puper{ L, -1 };
			puper.setIo(util::PuperIo::IN);
			manager.getComponentHandle(tid)->pupObject(puper, comp, {});
			
			lua_pop(L, 1);
		}

		lua_settop(L, 0);

		return 0;
	}

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

	//bool active(entity)
	int c_entity_active(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		lua_pushboolean(L, entity.isActive());

		return 1;
	}


	template<class T>
	int c_entity_addComponent(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);

		T& comp = entity.addComponent<T>();

		LuaPuper puper{ L, 2 };
		puper.setIo(util::PuperIo::IN);
		pup(puper, comp, {});
		
		lua_settop(L, 0);

		return 0;
	}

	template<class T>
	int c_entity_removeComponent(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		entity.removeComponent<T>();
	
		return 0;
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

	//entity createEntity(layer, name, table)
	int c_entityManager_createEntity(lua_State *L) {
		int argc = lua_gettop(L);
		uint8_t layer = static_cast<uint8_t>(toValue<uint32_t>(L, 1));
		std::string name;
		if (argc == 3) {
			name = toValue<std::string>(L, 2);
		}
		lua_rotate(L, -argc, 1);
		lua_settop(L, 1);
		
		lua_newtable(L);
		lua_rotate(L, -2, 1);
		//newtable table

		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");

		getRegistry(L, "entity");
		lua_setmetatable(L, -2);
		lua_setmetatable(L, -2);

		Entity e{ 0, nullptr };
		if (argc == 3) {
			e = Engine::inst().getSystem<ResourceManager>().require<Prefab>(name).createInstance(layer);
		} else {
			e = Engine::inst().getSystem<EntityManager>().createEntity(layer);
		}
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

	//int hash(string)
	int c_hash(lua_State *L) {
		std::string str = toValue<std::string>(L, 1);
		lua_settop(L, 0);

		pushValue(L, std::hash<std::string>{}(str));

		return 1;
	}

	void registerBindings(lua_State *L) {
		addFunctionToMetatable(L, "entity", "activate", c_entity_activate);
		addFunctionToMetatable(L, "entity", "deactivate", c_entity_deactivate);
		addFunctionToMetatable(L, "entity", "index", c_entity_index);
		addFunctionToMetatable(L, "entity", "layer", c_entity_layer);
		addFunctionToMetatable(L, "entity", "is_active", c_entity_active);

		addFunctionToMetatable(L, "entity", "addTransform", c_entity_addComponent<TransformComponent>);
		addFunctionToMetatable(L, "entity", "removeTransform", c_entity_removeComponent<TransformComponent>);
		addFunctionToMetatable(L, "entity", "getTransform", c_entity_getComponent<TransformComponent>);
		addFunctionToMetatable(L, "entity", "setTransform", c_entity_setComponent<TransformComponent>);

		addFunctionToMetatable(L, "entity", "addPhysicsBody2d", c_entity_addComponent<PhysicsBody2dComponent>);
		addFunctionToMetatable(L, "entity", "removePhysicsBody2d", c_entity_removeComponent<PhysicsBody2dComponent>);
		addFunctionToMetatable(L, "entity", "getPhysicsBody2d", c_entity_getComponent<PhysicsBody2dComponent>);
		addFunctionToMetatable(L, "entity", "setPhysicsBody2d", c_entity_setComponent<PhysicsBody2dComponent>);

		addFunctionToMetatable(L, "entity", "addAABB2d", c_entity_addComponent<AABB2dComponent>);
		addFunctionToMetatable(L, "entity", "removeAABB2d", c_entity_removeComponent<AABB2dComponent>);
		addFunctionToMetatable(L, "entity", "getAABB2d", c_entity_getComponent<AABB2dComponent>);
		addFunctionToMetatable(L, "entity", "setAABB2d", c_entity_setComponent<AABB2dComponent>);

		addFunctionToMetatable(L, "entity", "addSprite", c_entity_addComponent<SpriteComponent>);
		addFunctionToMetatable(L, "entity", "removeSprite", c_entity_removeComponent<SpriteComponent>);
		addFunctionToMetatable(L, "entity", "getSprite", c_entity_getComponent<SpriteComponent>);
		addFunctionToMetatable(L, "entity", "setSprite", c_entity_setComponent<SpriteComponent>);

		addFunctionToMetatable(L, "entity", "addText", c_entity_addComponent<TextComponent>);
		addFunctionToMetatable(L, "entity", "removeText", c_entity_removeComponent<TextComponent>);
		addFunctionToMetatable(L, "entity", "getText", c_entity_getComponent<TextComponent>);
		addFunctionToMetatable(L, "entity", "setText", c_entity_setComponent<TextComponent>);

		addFunctionToMetatable(L, "entity_manager", "create_prefab", c_create_prefab);
		addFunctionToMetatable(L, "entity_manager", "createEntity", c_entityManager_createEntity);
		addFunctionToMetatable(L, "entity_manager", "destroyEntity", c_entityManager_destroyEntity);

		lua_pushcfunction(L, c_hash);
		lua_setglobal(L, "hash");
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