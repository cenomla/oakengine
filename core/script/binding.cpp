#include "binding.h"

#include <string>

#include "graphics/opengl/gl_shader.h"
#include "graphics/opengl/gl_texture.h"
#include "graphics/opengl/gl_texture_atlas.h"
#include "graphics/opengl/gl_material.h"
#include "util/puper.h"
#include "script/lua_puper.h"
#include "resource_manager.h"
#include "view_system.h"
#include "events.h"
#include "components.h"
#include "engine.h"
#include "prefab.h"

namespace oak::luah {

	struct LuaShader {
		std::string name;
		std::string path;
	};

	static void pup(Puper &puper, LuaShader &data, const ObjInfo &info) {
		pup(puper, data.name, ObjInfo{ "name" } + info);
		pup(puper, data.path, ObjInfo{ "path" } + info);
	}

	struct LuaTexture {
		std::string name;
		std::string path;
		uint32_t flags = 0;
	};

	static void pup(Puper &puper, LuaTexture &data, const ObjInfo &info) {
		pup(puper, data.name, ObjInfo{ "name" } + info);
		pup(puper, data.path, ObjInfo{ "path" } + info);
		pup(puper, data.flags, ObjInfo{ "flags" } + info);
	}

	struct LuaAtlas {
		std::string name;
		std::vector<std::string> paths;
		glm::vec2 extent{ 0.0f };
		uint32_t flags = 0;
	};

	static void pup(Puper &puper, LuaAtlas &data, const ObjInfo &info) {
		pup(puper, data.name, ObjInfo{ "name" } + info);
		pup(puper, data.paths, ObjInfo{ "textures" } + info);
		pup(puper, data.extent, ObjInfo{ "extent" } + info);
		pup(puper, data.flags, ObjInfo{ "flags" } + info);
	}

	struct LuaMaterial {
		std::string name;
		std::string shader;
		std::string texture;
	};

	static void pup(Puper &puper, LuaMaterial &data, const ObjInfo &info) {
		pup(puper, data.name, ObjInfo{ "name" } + info);
		pup(puper, data.shader, ObjInfo{ "shader" } + info);
		pup(puper, data.texture, ObjInfo{ "texture" } + info);
	}

	struct LuaFont {
		std::string name;
		std::string material;
		std::string path;
	};

	static void pup(Puper &puper, LuaFont &data, const ObjInfo &info) {
		pup(puper, data.name, ObjInfo{ "name" } + info);
		pup(puper, data.material, ObjInfo{ "material" } + info);
		pup(puper, data.path, ObjInfo{ "path" } + info);
	}

	struct LuaSprite {
		std::string name;
		std::string material;
		glm::vec2 center{ 0.0f }, extent{ 0.0f }, draw{ 0.0f }, drawExtent{ 1.0f };
		std::string atlas;
		std::string path;
		int animFramesX = 1, animFramesY = 1;
	};

	static void pup(Puper &puper, LuaSprite &data, const ObjInfo &info) {
		pup(puper, data.name, ObjInfo{ "name" } + info);
		pup(puper, data.material, ObjInfo{ "material" } + info);
		pup(puper, data.center, ObjInfo{ "center" } + info);
		pup(puper, data.extent, ObjInfo{ "extent" } + info);
		pup(puper, data.draw, ObjInfo{ "draw_position" } + info);
		pup(puper, data.drawExtent, ObjInfo{ "draw_extent" } + info);
		pup(puper, data.atlas, ObjInfo{ "atlas" } + info);
		pup(puper, data.path, ObjInfo{ "path" } + info);
		pup(puper, data.animFramesX, ObjInfo{ "animframes_x" } + info);
		pup(puper, data.animFramesY, ObjInfo{ "animframes_y" } + info);
	}

	//void load_shader(table)
	static int c_resource_load_shader(lua_State *L) {
		LuaShader data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &shader = Engine::inst().getSystem<ResourceManager>().add<graphics::GLShader>(data.name);
		shader.create(data.path + "/opengl.vert", data.path + "/opengl.frag");

		return 0;
	}

	//void load_texture(table)
	static int c_resource_load_texture(lua_State *L) {
		LuaTexture data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &texture = Engine::inst().getSystem<ResourceManager>().add<graphics::GLTexture>(data.name, GLenum{ GL_TEXTURE_2D }, (data.flags & 0x1 ) == 0x1 ? GLenum{ GL_LINEAR } : GLenum{ GL_NEAREST });
		texture.create(data.path + ".png");
	
		return 0;
	}

	//void load_atlas(table)
	static int c_resource_load_atlas(lua_State *L) {
		LuaAtlas data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &atlas = Engine::inst().getSystem<ResourceManager>().add<graphics::GLTextureAtlas>(data.name, GLenum{ GL_TEXTURE_2D }, (data.flags & 0x1 ) == 0x1 ? GLenum{ GL_LINEAR } : GLenum{ GL_NEAREST });
		for (auto &it : data.paths) {
			atlas.addTexture(it + ".png");
		}
		atlas.bake(static_cast<int>(data.extent.x), static_cast<int>(data.extent.y));
	
		return 0;
	}

	//void load_material(table)
	static int c_resource_load_material(lua_State *L) {
		LuaMaterial data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &resManager = Engine::inst().getSystem<ResourceManager>();

		auto shader = resManager.try_require<graphics::GLShader>(std::hash<std::string>{}(data.shader));
		const graphics::GLTexture *texture = resManager.try_require<graphics::GLTextureAtlas>(std::hash<std::string>{}(data.texture));
		if (texture == nullptr) {
			texture = resManager.try_require<graphics::GLTexture>(std::hash<std::string>{}(data.texture));
		}

		if (shader == nullptr || texture == nullptr) {
			log::cout << "invalid material: " << data.name << std::endl;
			abort();
		}

		resManager.add<graphics::GLMaterial>(data.name, std::hash<std::string>{}(data.name), shader, texture);

		return 0;
	}

	//void load_font(table)
	static int c_resource_load_font(lua_State *L) {
		LuaFont data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &font = Engine::inst().getSystem<ResourceManager>().add<graphics::Font>(data.name, std::hash<std::string>{}(data.material));
		font.create(data.path + "/glyphs.fnt");
	
		return 0;
	}

	//void load_sprite(table)
	static int c_resource_load_sprite(lua_State *L) {
		LuaSprite data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &resManager = Engine::inst().getSystem<ResourceManager>();

		if (data.atlas.empty()) {
			resManager.add<graphics::Sprite>(data.name, std::hash<std::string>{}(data.material), data.center.x, data.center.y, data.extent.x, data.extent.y, data.draw.x, data.draw.y, data.drawExtent.x, data.drawExtent.y);
		} else {
			const auto &atlas = resManager.require<graphics::GLTextureAtlas>(data.atlas);
			resManager.add<graphics::Sprite>(data.name, std::hash<std::string>{}(data.material), data.center.x, data.center.y, data.extent.x, data.extent.y, atlas.getTextureRegion(data.path + ".png"), data.animFramesX, data.animFramesY);
		}

		return 0;

	}

	//void set_uniform(shader, name, value)
	static int c_resource_set_uniform(lua_State *L) {
		const std::string shaderName{ lua_tostring(L, 1) };
		const std::string uniformName{ lua_tostring(L, 2) };

		auto &resManager = Engine::inst().getSystem<ResourceManager>();
		const auto& shader = resManager.require<graphics::GLShader>(shaderName);
		shader.bind();

		switch (lua_type(L, 3)) {
			case LUA_TNUMBER:
				if (lua_isinteger(L, 3)) {
					shader.setUniform(uniformName, static_cast<uint32_t>(lua_tointeger(L, 3)));
				} else {
					shader.setUniform(uniformName, static_cast<float>(lua_tonumber(L, 3)));
				}
			break;
			case LUA_TTABLE:
				lua_len(L, 3);
				if (lua_tointeger(L, -1) == 3) {
					lua_pop(L, 1);
					lua_geti(L, 3, 1);
					lua_geti(L, 3, 2);
					lua_geti(L, 3, 3);
					shader.setUniform(uniformName, glm::vec3{ lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6) });
				}
			break;
		}

		lua_settop(L, 0);
		return 0;
	}


	//void create_prefab(name, table)
	static int c_create_prefab(lua_State *L) {
		EntityManager &manager = oak::Engine::inst().getSystem<EntityManager>();
		std::string name = toValue<std::string>(L, 1);
		lua_pushvalue(L, 2);
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
			puper.setIo(PuperIo::IN);
			manager.getComponentHandle(tid)->pupObject(puper, comp, {});
			
			lua_pop(L, 1);
		}

		lua_settop(L, 0);

		return 0;
	}

	//void activate(entity)
	static int c_entity_activate(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		entity.activate();

		return 0;
	}

	//void deactivate(entity)
	static int c_entity_deactivate(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		entity.deactivate();

		return 0;
	}

	//uint index(entity)
	static int c_entity_index(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		pushValue(L, entity.index());
	
		return 1;
	}

	//uint layer(entity)
	static int c_entity_layer(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		pushValue(L, entity.layer());

		return 1;
	}

	//bool active(entity)
	static int c_entity_active(lua_State *L) {
		Entity entity = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		lua_pushboolean(L, entity.isActive());

		return 1;
	}

	//entity create_entity(layer, name, table)
	static int c_entityManager_createEntity(lua_State *L) {
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

	//void destroy_entity(entity)
	static int c_entityManager_destroyEntity(lua_State *L) {
		Entity e = toValue<Entity>(L, 1);

		lua_settop(L, 0);

		e.destroy();
		return 0;
	}

	//void define_view(viewId, layers)
	static int c_view_defineView(lua_State *L) {
		ViewSystem& viewSystem = Engine::inst().getSystem<ViewSystem>();

		size_t viewId = toValue<size_t>(L, 1);

		std::vector<uint32_t> layers;

		LuaPuper puper{ L, 2 };
		puper.setIo(PuperIo::IN);
		pup(puper, layers, {});

		viewSystem.defineView(viewId, layers);

		lua_settop(L, 0);

		return 0;

	}

	//void set_view(viewId, view)
	static int c_view_setView(lua_State *L) {
		ViewSystem& viewSystem = Engine::inst().getSystem<ViewSystem>();

		size_t viewId = toValue<size_t>(L, 1);

		View view = viewSystem.getView(viewId);
		LuaPuper puper{ L, 2 };
		puper.setIo(PuperIo::IN);
		pup(puper, view, {});

		viewSystem.setView(viewId, view);

		lua_settop(L, 0);

		return 0;
	}

	//void view_get_view(viewId)
	static int c_view_getView(lua_State *L) {
		ViewSystem& viewSystem = Engine::inst().getSystem<ViewSystem>();
		size_t viewId = toValue<size_t>(L, 1);

		lua_settop(L, 0);
		lua_newtable(L);

		View view = viewSystem.getView(viewId);
		LuaPuper puper{ L, 1 };
		pup(puper, view, {});

		return 1;
	}

	//vec2 view_transform_point(viewId, point)
	static int c_view_transformPoint(lua_State *L) {
		ViewSystem& vs = Engine::inst().getSystem<ViewSystem>();

		size_t viewId = toValue<size_t>(L, 1);

		LuaPuper puper{ L, 2 };
		puper.setIo(PuperIo::IN);
		glm::vec2 point;
		pup(puper, point, {});

		lua_settop(L, 0);

		point = vs.transformPoint(viewId, point);

		lua_newtable(L);

		puper.setIndex(1);
		puper.setIo(PuperIo::OUT);

		pup(puper, point, {});

		return 1;
	}

	//int view_get_id(layer)
	static int c_view_getId(lua_State *L) {
		ViewSystem& vs = Engine::inst().getSystem<ViewSystem>();

		uint32_t layer = toValue<uint32_t>(L, 1);
		lua_settop(L, 0);

		size_t id = vs.getViewId(layer);

		pushValue(L, id);

		return 1;
	}

	//int hash(string)
	static int c_hash(lua_State *L) {
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

		addFunctionToMetatable(L, "entity", "add_transform", c_entity_addComponent<TransformComponent>);
		addFunctionToMetatable(L, "entity", "remove_transform", c_entity_removeComponent<TransformComponent>);
		addFunctionToMetatable(L, "entity", "get_transform", c_entity_getComponent<TransformComponent>);
		addFunctionToMetatable(L, "entity", "set_transform", c_entity_setComponent<TransformComponent>);
		addFunctionToMetatable(L, "entity", "has_transform", c_entity_hasComponent<TransformComponent>);

		addFunctionToMetatable(L, "entity", "add_physicsBody2d", c_entity_addComponent<PhysicsBody2dComponent>);
		addFunctionToMetatable(L, "entity", "remove_physicsBody2d", c_entity_removeComponent<PhysicsBody2dComponent>);
		addFunctionToMetatable(L, "entity", "get_physicsBody2d", c_entity_getComponent<PhysicsBody2dComponent>);
		addFunctionToMetatable(L, "entity", "set_physicsBody2d", c_entity_setComponent<PhysicsBody2dComponent>);
		addFunctionToMetatable(L, "entity", "has_physicsBody2d", c_entity_hasComponent<PhysicsBody2dComponent>);

		addFunctionToMetatable(L, "entity", "add_aabb2d", c_entity_addComponent<AABB2dComponent>);
		addFunctionToMetatable(L, "entity", "remove_aabb2d", c_entity_removeComponent<AABB2dComponent>);
		addFunctionToMetatable(L, "entity", "get_aabb2d", c_entity_getComponent<AABB2dComponent>);
		addFunctionToMetatable(L, "entity", "set_aabb2d", c_entity_setComponent<AABB2dComponent>);
		addFunctionToMetatable(L, "entity", "has_aabb2d", c_entity_hasComponent<AABB2dComponent>);

		addFunctionToMetatable(L, "entity", "add_sprite", c_entity_addComponent<SpriteComponent>);
		addFunctionToMetatable(L, "entity", "remove_sprite", c_entity_removeComponent<SpriteComponent>);
		addFunctionToMetatable(L, "entity", "get_sprite", c_entity_getComponent<SpriteComponent>);
		addFunctionToMetatable(L, "entity", "set_sprite", c_entity_setComponent<SpriteComponent>);
		addFunctionToMetatable(L, "entity", "has_sprite", c_entity_hasComponent<SpriteComponent>);

		addFunctionToMetatable(L, "entity", "add_text", c_entity_addComponent<TextComponent>);
		addFunctionToMetatable(L, "entity", "remove_text", c_entity_removeComponent<TextComponent>);
		addFunctionToMetatable(L, "entity", "get_text", c_entity_getComponent<TextComponent>);
		addFunctionToMetatable(L, "entity", "set_text", c_entity_setComponent<TextComponent>);
		addFunctionToMetatable(L, "entity", "has_text", c_entity_hasComponent<TextComponent>);

		addFunctionToMetatable(L, "entity_manager", "create_prefab", c_create_prefab);
		addFunctionToMetatable(L, "entity_manager", "create_entity", c_entityManager_createEntity);
		addFunctionToMetatable(L, "entity_manager", "destroy_entity", c_entityManager_destroyEntity);

		addFunctionToMetatable(L, "oak", "load_shader", c_resource_load_shader);
		addFunctionToMetatable(L, "oak", "load_texture", c_resource_load_texture);
		addFunctionToMetatable(L, "oak", "load_atlas", c_resource_load_atlas);
		addFunctionToMetatable(L, "oak", "load_material", c_resource_load_material);
		addFunctionToMetatable(L, "oak", "load_font", c_resource_load_font);
		addFunctionToMetatable(L, "oak", "load_sprite", c_resource_load_sprite);
		addFunctionToMetatable(L, "oak", "set_uniform", c_resource_set_uniform);

		addFunctionToMetatable(L, "oak", "view_transform_point", c_view_transformPoint);
		addFunctionToMetatable(L, "oak", "view_get_id", c_view_getId);
		addFunctionToMetatable(L, "oak", "view_define_view", c_view_defineView);
		addFunctionToMetatable(L, "oak", "view_set_view", c_view_setView);
		addFunctionToMetatable(L, "oak", "view_get_view", c_view_getView);

		lua_pushcfunction(L, c_hash);
		lua_setglobal(L, "hash");

		auto &engine = Engine::inst();

		//setup event listeners
		engine.getEventManager().add<oak::KeyEvent>([L](oak::KeyEvent evt) {
			oak::luah::getGlobal(L, "oak.es.emit_event");
			oak::luah::getGlobal(L, "oak.es");
			oak::luah::pushValue(L, "on_key_press");
			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});
			oak::luah::call(L, 3, 0);
		});

		engine.getEventManager().add<oak::ButtonEvent>([L](oak::ButtonEvent evt){
			oak::luah::getGlobal(L, "oak.es.emit_event");
			oak::luah::getGlobal(L, "oak.es");
			oak::luah::pushValue(L, "on_button_press");
			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});
			oak::luah::call(L, 3, 0);
		});
		
		engine.getEventManager().add<oak::MouseMoveEvent>([L](oak::MouseMoveEvent evt) {
			oak::luah::getGlobal(L, "oak.es.emit_event");
			oak::luah::getGlobal(L, "oak.es");
			oak::luah::pushValue(L, "on_mouse_move");
			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});
			oak::luah::call(L, 3, 0);
		});

		engine.getEventManager().add<oak::CharEvent>([L](oak::CharEvent evt) {
			oak::luah::getGlobal(L, "oak.es.emit_event");
			oak::luah::getGlobal(L, "oak.es");
			oak::luah::pushValue(L, "on_char_press");
			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});
			oak::luah::call(L, 3, 0);
		});

		engine.getEventManager().add<oak::EntityActivateEvent>([L](const oak::EntityActivateEvent &evt) {
			oak::luah::getGlobal(L, "oak.es.send_message");
			oak::luah::getGlobal(L, "oak.es");
			oak::luah::pushValue(L, "on_activate");
			oak::luah::pushValue(L, evt.entity.index());
			oak::luah::call(L, 3, 0);
		});

		engine.getEventManager().add<oak::EntityDeactivateEvent>([L](const oak::EntityDeactivateEvent &evt) {
			oak::luah::getGlobal(L, "oak.es.send_message");
			oak::luah::getGlobal(L, "oak.es");
			oak::luah::pushValue(L, "on_deactivate");
			oak::luah::pushValue(L, evt.entity.index());
			oak::luah::call(L, 3, 0);
		});

		engine.getEventManager().add<oak::EntityCollisionEvent>([L](oak::EntityCollisionEvent evt) {
			oak::luah::getGlobal(L, "oak.es.send_event");
			oak::luah::getGlobal(L, "oak.es");
			oak::luah::pushValue(L, "on_entity_collide");

			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});

			oak::luah::call(L, 3, 0);
		});
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

}