#include "binding.h"

#include <string>

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

	//x, y, z getPosition(entity)
	int c_entity_getPosition(lua_State *L) {
		//get args
		Entity entity = toValue<Entity>(L, 1);
		//clear args
		lua_settop(L, 0);

		const auto& tc = entity.getComponent<TransformComponent>();

		lua_pushnumber(L, tc.position.x);
		lua_pushnumber(L, tc.position.y);
		lua_pushnumber(L, tc.position.z);

		return 3;
	}

	//void setPosition(entity, x, y, z)
	int c_entity_setPosition(lua_State *L) {
		//get args
		Entity entity = toValue<Entity>(L, 1);

		auto& tc = entity.getComponent<TransformComponent>();

		float x = toValue<float>(L, 2);
		float y = toValue<float>(L, 3);
		float z = isNil(L, 4) ? tc.position.z : toValue<float>(L, 4);

		//clear args
		lua_settop(L, 0);

		tc.position = glm::vec3{ x, y, z };
		
		return 0;
	}

	//vx, vy, getVelocity(entity)
	int c_entity_getVelocity(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		auto &pc = entity.getComponent<PhysicsBody2dComponent>();

		lua_pushnumber(L, pc.velocity.x);
		lua_pushnumber(L, pc.velocity.y);

		return 2;
	}

	//void setVelocity(entity, vx, vy)
	int c_entity_setVelocity(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		float vx = toValue<float>(L, 2);
		float vy = toValue<float>(L, 3);
		lua_settop(L, 0);

		auto &pc = entity.getComponent<PhysicsBody2dComponent>();

		pc.velocity = glm::vec2{ vx, vy };

		return 0;
	}

	//void addForce(entity, fx, fy)
	int c_entity_addForce(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		float fx = toValue<float>(L, 2);
		float fy = toValue<float>(L, 3);
		lua_settop(L, 0);

		auto &pc = entity.getComponent<PhysicsBody2dComponent>();

		pc.force += glm::vec2{ fx, fy };

		return 0;
	}

	//float getMass(entity)
	int c_entity_getMass(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);
		auto &pc = entity.getComponent<PhysicsBody2dComponent>();
		lua_pushnumber(L, pc.mass);
		return 1;
	}

	//void setText(entity, text)
	int c_entity_setText(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		std::string text{ lua_tostring(L, 2) };
		lua_settop(L, 0);
		size_t id = std::hash<std::string>{}(text);
		auto &tc = entity.getComponent<Resource<std::string>>();
		tc = Resource<std::string>{ id };

		return 0;
	}

	//entity createEntity(layer, name)
	int c_entityManager_createEntity(lua_State *L) {
		uint8_t layer = toValue<uint8_t>(L, 1);
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
		addFunctionToMetatable(L, "entity", "getPosition", c_entity_getPosition);
		addFunctionToMetatable(L, "entity", "setPosition", c_entity_setPosition);
		addFunctionToMetatable(L, "entity", "getVelocity", c_entity_getVelocity);
		addFunctionToMetatable(L, "entity", "setVelocity", c_entity_setVelocity);
		addFunctionToMetatable(L, "entity", "addForce", c_entity_addForce);
		addFunctionToMetatable(L, "entity", "getMass", c_entity_getMass);
		addFunctionToMetatable(L, "entity", "setText", c_entity_setText);

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
		pushInstance<EntityManager>(L, entityManager);
		setMetatable(L, "entity_manager");
	}

	template<> EntityManager& toValue(lua_State *L, int idx) {
		return toInstance<EntityManager>(L, idx);
	}

}