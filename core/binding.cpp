#include "binding.h"

#include "components.h"

namespace oak::luah {

	//x, y, z getPosition(entity)
	int getPosition(lua_State *L) {
		//get args
		oak::Entity entity = luah::toValue<oak::Entity>(L, 1);
		//clear args
		lua_settop(L, 0);

		const auto& tc = entity.getComponent<TransformComponent>();

		lua_pushnumber(L, tc.position.x);
		lua_pushnumber(L, tc.position.y);
		lua_pushnumber(L, tc.position.z);

		return 3;
	}

	//void setPosition(entity, x, y, z)
	int setPosition(lua_State *L) {
		//get args
		oak::Entity entity = luah::toValue<oak::Entity>(L, 1);

		auto& tc = entity.getComponent<TransformComponent>();

		float x = toValue<float>(L, 2);
		float y = toValue<float>(L, 3);
		float z = isNil(L, 4) ? tc.position.z : toValue<float>(L, 4);

		//clear args
		lua_settop(L, 0);

		tc.position = glm::vec3{ x, y, z };
		
		return 0;
	}

	//entity createEntity(manager, layer, name)
	int createEntity(lua_State *L) {
		EntityManager &manager = toValue<EntityManager&>(L, 1);
		uint8_t layer = toValue<uint8_t>(L, 2);
		std::string name = toValue<std::string>(L, 3);
		//clear stack
		lua_settop(L, 0);

		lua_newtable(L);
		getRegistry(L, "prefabs." + name);
		lua_setmetatable(L, -2);

		Entity e = manager.createEntity(layer);
		pushValue(L, e);

		lua_setfield(L, -2, "_id");
		

		return 1;
	}

	//void destroyEntity(entity)
	int destroyEntity(lua_State *L) {
		Entity e = toValue<Entity>(L, 1);

		lua_settop(L, 0);

		e.destroy();
		return 0;
	}

	void registerBindings(lua_State *L) {
		luah::addFunctionToMetatable(L, "entity", "getPosition", getPosition);
		luah::addFunctionToMetatable(L, "entity", "setPosition", setPosition);

		luah::addFunctionToMetatable(L, "entity_manager", "createEntity", createEntity);
		luah::addFunctionToMetatable(L, "entity_manager", "destroyEntity", destroyEntity);

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
		pushInstance<EntityManager>(L, entityManager);
		setMetatable(L, "entity_manager");
	}

	template<> EntityManager& toValue(lua_State *L, int idx) {
		return toInstance<EntityManager>(L, idx);
	}

}