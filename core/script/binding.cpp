#include "binding.h"


#include "container.h"
#include "graphics/opengl/gl_shader.h"
#include "graphics/opengl/gl_texture.h"
#include "graphics/opengl/gl_texture_atlas.h"
#include "graphics/opengl/gl_material.h"
#include "util/puper.h"
#include "script/lua_puper.h"
#include "resource_manager.h"
#include "file_manager.h"
#include "view_system.h"
#include "debugger.h"
#include "save_manager.h"
#include "events.h"
#include "components.h"
#include "engine.h"
#include "prefab.h"
#include "save_funcs.h"

namespace oak::luah {

	struct LuaShader {
		oak::string name;
		oak::string path;
	};

	static void pup(Puper &puper, LuaShader &data, const ObjInfo &info) {
		pup(puper, data.name, info + ObjInfo{ "name" });
		pup(puper, data.path, info + ObjInfo{ "path" });
	}

	struct LuaTexture {
		oak::string name;
		oak::string path;
		uint32_t flags = 0;
	};

	static void pup(Puper &puper, LuaTexture &data, const ObjInfo &info) {
		pup(puper, data.name, info + ObjInfo{ "name" });
		pup(puper, data.path, info + ObjInfo{ "path" });
		pup(puper, data.flags, info + ObjInfo{ "flags" });
	}

	struct LuaAtlas {
		oak::string name;
		oak::vector<oak::string> paths;
		glm::vec2 extent{ 0.0f };
		uint32_t flags = 0;
	};

	static void pup(Puper &puper, LuaAtlas &data, const ObjInfo &info) {
		pup(puper, data.name, info + ObjInfo{ "name" });
		pup(puper, data.paths, info + ObjInfo{ "textures" });
		pup(puper, data.extent, info + ObjInfo{ "extent" });
		pup(puper, data.flags, info + ObjInfo{ "flags" });
	}

	struct LuaMaterial {
		oak::string name;
		oak::string shader;
		oak::string texture;
	};

	static void pup(Puper &puper, LuaMaterial &data, const ObjInfo &info) {
		pup(puper, data.name, info + ObjInfo{ "name" });
		pup(puper, data.shader, info + ObjInfo{ "shader" });
		pup(puper, data.texture, info + ObjInfo{ "texture" });
	}

	struct LuaFont {
		oak::string name;
		oak::string material;
		oak::string path;
	};

	static void pup(Puper &puper, LuaFont &data, const ObjInfo &info) {
		pup(puper, data.name, info + ObjInfo{ "name" });
		pup(puper, data.material, info + ObjInfo{ "material" });
		pup(puper, data.path, info + ObjInfo{ "path" });
	}

	struct LuaSprite {
		oak::string name;
		oak::string material;
		glm::vec2 center{ 0.0f }, extent{ 0.0f }, draw{ 0.0f }, drawExtent{ 1.0f };
		oak::string atlas;
		oak::string path;
		int animFramesX = 1, animFramesY = 1;
	};

	static void pup(Puper &puper, LuaSprite &data, const ObjInfo &info) {
		pup(puper, data.name, info + ObjInfo{ "name" });
		pup(puper, data.material, info + ObjInfo{ "material" });
		pup(puper, data.center, info + ObjInfo{ "center" });
		pup(puper, data.extent, info + ObjInfo{ "extent" });
		pup(puper, data.draw, info + ObjInfo{ "draw_position" });
		pup(puper, data.drawExtent, info + ObjInfo{ "draw_extent" });
		pup(puper, data.atlas, info + ObjInfo{ "atlas" });
		pup(puper, data.path, info + ObjInfo{ "path" });
		pup(puper, data.animFramesX, info + ObjInfo{ "animframes_x" });
		pup(puper, data.animFramesY, info + ObjInfo{ "animframes_y" });
	}

	//void load_shader(table)
	static int c_resource_load_shader(lua_State *L) {
		LuaShader data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &shader = Engine::inst().getSystem<ResourceManager>().add<graphics::GLShader>(data.name);
		oak::string path = Engine::inst().getSystem<FileManager>().resolve(data.path);
		shader.create(path + "/opengl.vert", path + "/opengl.frag");

		return 0;
	}

	//void load_texture(table)
	static int c_resource_load_texture(lua_State *L) {
		LuaTexture data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &texture = Engine::inst().getSystem<ResourceManager>().add<graphics::GLTexture>(data.name, GLenum{ GL_TEXTURE_2D }, (data.flags & 0x1 ) == 0x1 ? GLenum{ GL_LINEAR } : GLenum{ GL_NEAREST });
		oak::string path = Engine::inst().getSystem<FileManager>().resolve(data.path);
		texture.create(path + ".png");
	
		return 0;
	}

	//void load_atlas(table)
	static int c_resource_load_atlas(lua_State *L) {
		LuaAtlas data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &atlas = Engine::inst().getSystem<ResourceManager>().add<graphics::GLTextureAtlas>(data.name, GLenum{ GL_TEXTURE_2D }, (data.flags & 0x1 ) == 0x1 ? GLenum{ GL_LINEAR } : GLenum{ GL_NEAREST });
		oak::string path;
		for (auto &it : data.paths) {
			path = Engine::inst().getSystem<FileManager>().resolve(it);
			atlas.addTexture(path + ".png");
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

		auto shader = resManager.try_require<graphics::GLShader>(std::hash<oak::string>{}(data.shader));
		const graphics::GLTexture *texture = resManager.try_require<graphics::GLTextureAtlas>(std::hash<oak::string>{}(data.texture));
		if (texture == nullptr) {
			texture = resManager.try_require<graphics::GLTexture>(std::hash<oak::string>{}(data.texture));
		}

		if (shader == nullptr || texture == nullptr) {
			log_print_err("invalid material: %s", data.name.c_str());
			abort();
		}

		resManager.add<graphics::GLMaterial>(data.name, std::hash<oak::string>{}(data.name), shader, texture);

		return 0;
	}

	//void load_font(table)
	static int c_resource_load_font(lua_State *L) {
		LuaFont data;

		LuaPuper puper{ L, 1 };
		puper.setIo(PuperIo::IN);
		pup(puper, data, {});

		auto &font = Engine::inst().getSystem<ResourceManager>().add<graphics::Font>(data.name, std::hash<oak::string>{}(data.material));
		oak::string path = Engine::inst().getSystem<FileManager>().resolve(data.path);
		font.create(path + "/glyphs.fnt");
	
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
			resManager.add<graphics::Sprite>(data.name, std::hash<oak::string>{}(data.material), data.center.x, data.center.y, data.extent.x, data.extent.y, data.draw.x, data.draw.y, data.drawExtent.x, data.drawExtent.y);
		} else {
			const auto &atlas = resManager.require<graphics::GLTextureAtlas>(data.atlas);
			oak::string path = Engine::inst().getSystem<FileManager>().resolve(data.path);
			resManager.add<graphics::Sprite>(data.name, std::hash<oak::string>{}(data.material), data.center.x, data.center.y, data.extent.x, data.extent.y, atlas.getTextureRegion(path + ".png"), data.animFramesX, data.animFramesY);
		}

		return 0;

	}

	//void set_uniform(shader, name, value)
	static int c_resource_set_uniform(lua_State *L) {
		const oak::string shaderName{ lua_tostring(L, 1) };
		const oak::string uniformName{ lua_tostring(L, 2) };

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
	
	//void load_prefab(name, prefab, script)
	static int c_resource_load_prefab(lua_State *L) {
		EntityManager &manager = oak::Engine::inst().getSystem<EntityManager>();
		oak::string name = toValue<oak::string>(L, 1);
		oak::vector<oak::string> keys{ oak::frameAllocator };
		getKeys(L, 2, keys);

		//setup script metatable
		if (lua_gettop(L) < 3) {
			lua_newtable(L);
		}

		//name prefab script script
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		lua_getglobal(L, "prefab_newindex");
		lua_setfield(L, -2, "__newindex");

		//script
		//script entity
		lua_getfield(L, LUA_REGISTRYINDEX, "entity");
		lua_setmetatable(L, -2);
		//script 
		//script _prefabs_
		lua_getfield(L, LUA_REGISTRYINDEX, "_prefabs_");
		lua_rotate(L, -2, 1);
		//_prefabs_ script
		lua_setfield(L, -2, name.c_str());
		lua_pop(L, 1);
		
		//create a prefab from the table data
		auto &prefab = Engine::inst().getSystem<ResourceManager>().add<Prefab>(name, &manager);
		auto &chs = Engine::inst().getSystem<ComponentHandleStorage>();

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
			size_t tid = chs.getId(k);
			void *comp = prefab.addComponent(shared, tid);

			LuaPuper puper{ L, -1 };
			puper.setIo(PuperIo::IN);
			chs.getHandle(tid)->pupObject(puper, comp, {});
			
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

	//entity_table create_entity(layer, name/table)
	static int c_entityManager_createEntity(lua_State *L) {
		uint8_t layer = static_cast<uint8_t>(toValue<uint32_t>(L, 1));
		lua_remove(L, 1);
		
		if (lua_isstring(L, 1)) { //create prefab instance
			oak::string name = toValue<oak::string>(L, 1);
			lua_settop(L, 0);
			lua_newtable(L);
			getField(L, LUA_REGISTRYINDEX, oak::string{ "_prefabs_." + name, oak::frameAllocator });
			lua_setmetatable(L, -2);

			pushValue(L, Engine::inst().getSystem<ResourceManager>().require<Prefab>(name).createInstance(layer));

			lua_setfield(L, -2, "_id");
		} else { //create prefabless entity
			getField(L, LUA_REGISTRYINDEX, oak::string{ "entity", oak::frameAllocator });
			lua_setmetatable(L, -2);

			pushValue(L, Engine::inst().getSystem<EntityManager>().createEntity(layer));

			lua_setfield(L, -2, "_id");
		}

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

		oak::vector<uint32_t> layers;

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
		oak::string str = toValue<oak::string>(L, 1);
		lua_settop(L, 0);

		pushValue(L, std::hash<oak::string>{}(str));

		return 1;
	}

	//table get_debug_vars()
	static int c_debug_getVars(lua_State *L) {
		lua_newtable(L);
		LuaPuper puper{ L, 1 };
		pup(puper, debugVars, {});

		return 1;
	}

	//string memory_string(bytes)
	static int c_memory_getString(lua_State *L) {
		size_t data = luah::toValue<size_t>(L, 1);
		lua_settop(L, 0);
		luah::pushValue(L, MemoryManager::memoryString(data).c_str());
		return 1;
	}

	//void open(path, flags)
	static int c_save_open(lua_State *L) {
		oak::string path = toValue<oak::string>(L, 1);
		uint32_t flags = toValue<uint32_t>(L, 2);
		lua_settop(L, 0);

		Engine::inst().getSystem<SaveManager>().open(path, flags);

		return 0;
	}

	//void close()
	static int c_save_close(lua_State *L) {
		Engine::inst().getSystem<SaveManager>().close();
		return 0;
	}

	//bool is_open()
	static int c_save_isOpen(lua_State *L) {
		lua_pushboolean(L, Engine::inst().getSystem<SaveManager>().isOpen());
		return 1;
	}

	//void stream_entity(entity)
	static int c_save_streamEntity(lua_State *L) {
		Entity e = toValue<Entity>(L, 1);
		lua_settop(L, 0);

		save::streamEntity(Engine::inst().getSystem<SaveManager>(), e);

		return 0;
	}

	//void quit()
	static int c_quit(lua_State *L) {
		Engine::inst().getEventManager().emitEvent(QuitEvent{});
		return 0;
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

		addFunctionToMetatable(L, "entity_manager", "create_entity", c_entityManager_createEntity);
		addFunctionToMetatable(L, "entity_manager", "destroy_entity", c_entityManager_destroyEntity);

		addFunctionToMetatable(L, "save_manager", "open", c_save_open);
		addFunctionToMetatable(L, "save_manager", "is_open", c_save_isOpen);
		addFunctionToMetatable(L, "save_manager", "close", c_save_close);
		addFunctionToMetatable(L, "save_manager", "stream_entity", c_save_streamEntity);

		addFunctionToMetatable(L, "oak", "load_shader", c_resource_load_shader);
		addFunctionToMetatable(L, "oak", "load_texture", c_resource_load_texture);
		addFunctionToMetatable(L, "oak", "load_atlas", c_resource_load_atlas);
		addFunctionToMetatable(L, "oak", "load_material", c_resource_load_material);
		addFunctionToMetatable(L, "oak", "load_font", c_resource_load_font);
		addFunctionToMetatable(L, "oak", "load_sprite", c_resource_load_sprite);
		addFunctionToMetatable(L, "oak", "set_uniform", c_resource_set_uniform);
		addFunctionToMetatable(L, "oak", "load_prefab", c_resource_load_prefab);
		addFunctionToMetatable(L, "oak", "debug_get_vars", c_debug_getVars);
		addFunctionToMetatable(L, "oak", "memory_string", c_memory_getString);
		addFunctionToMetatable(L, "oak", "quit", c_quit);

		addFunctionToMetatable(L, "view_system", "transform_point", c_view_transformPoint);
		addFunctionToMetatable(L, "view_system", "get_id", c_view_getId);
		addFunctionToMetatable(L, "view_system", "define_view", c_view_defineView);
		addFunctionToMetatable(L, "view_system", "set_view", c_view_setView);
		addFunctionToMetatable(L, "view_system", "get_view", c_view_getView);

		lua_getglobal(L, "oak");
		//view system
		lua_newtable(L);
		setMetatable(L, "view_system");
		lua_setfield(L, -2, "vs");
		//save manager
		lua_newtable(L);
		setMetatable(L, "save_manager");
		lua_setfield(L, -2, "save");

		lua_pop(L, 1);

		lua_pushcfunction(L, c_hash);
		lua_setglobal(L, "hash");

		auto &engine = Engine::inst();

		//setup event listeners
		engine.getEventManager().add<oak::KeyEvent>([L](oak::KeyEvent evt) {
			oak::luah::getGlobal(L, oak::string{ "oak.es.emit_event", oak::frameAllocator });
			oak::luah::getGlobal(L, oak::string{ "oak.es", oak::frameAllocator });
			oak::luah::pushValue(L, oak::string{ "on_key_press", oak::frameAllocator });
			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});
			oak::luah::call(L, 3, 0);
		});

		engine.getEventManager().add<oak::ButtonEvent>([L](oak::ButtonEvent evt){
			oak::luah::getGlobal(L, oak::string{ "oak.es.emit_event", oak::frameAllocator });
			oak::luah::getGlobal(L, oak::string{ "oak.es", oak::frameAllocator });
			oak::luah::pushValue(L, oak::string{ "on_button_press", oak::frameAllocator });
			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});
			oak::luah::call(L, 3, 0);
		});
		
		engine.getEventManager().add<oak::MouseMoveEvent>([L](oak::MouseMoveEvent evt) {
			oak::luah::getGlobal(L, oak::string{ "oak.es.emit_event", oak::frameAllocator });
			oak::luah::getGlobal(L, oak::string{ "oak.es", oak::frameAllocator });
			oak::luah::pushValue(L, oak::string{ "on_mouse_move", oak::frameAllocator });
			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});
			oak::luah::call(L, 3, 0);
		});

		engine.getEventManager().add<oak::CharEvent>([L](oak::CharEvent evt) {
			oak::luah::getGlobal(L, oak::string{ "oak.es.emit_event", oak::frameAllocator });
			oak::luah::getGlobal(L, oak::string{ "oak.es", oak::frameAllocator });
			oak::luah::pushValue(L, oak::string{ "on_char_press", oak::frameAllocator });
			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});
			oak::luah::call(L, 3, 0);
		});

		engine.getEventManager().add<oak::EntityActivateEvent>([L](const oak::EntityActivateEvent &evt) {
			oak::luah::getGlobal(L, oak::string{ "oak.es.send_message", oak::frameAllocator });
			oak::luah::getGlobal(L, oak::string{ "oak.es", oak::frameAllocator });
			oak::luah::pushValue(L, oak::string{ "on_activate", oak::frameAllocator });
			oak::luah::pushValue(L, evt.entity.index());
			oak::luah::call(L, 3, 0);
		});

		engine.getEventManager().add<oak::EntityDeactivateEvent>([L](const oak::EntityDeactivateEvent &evt) {
			oak::luah::getGlobal(L, oak::string{ "oak.es.send_message", oak::frameAllocator });
			oak::luah::getGlobal(L, oak::string{ "oak.es", oak::frameAllocator });
			oak::luah::pushValue(L, oak::string{ "on_deactivate", oak::frameAllocator });
			oak::luah::pushValue(L, evt.entity.index());
			oak::luah::call(L, 3, 0);
		});
		/*
		engine.getEventManager().add<oak::EntityCollisionEvent>([L](oak::EntityCollisionEvent evt) {
			oak::luah::getGlobal(L, oak::string{ "oak.es.send_message", oak::frameAllocator });
			oak::luah::getGlobal(L, oak::string{ "oak.es", oak::frameAllocator });
			oak::luah::pushValue(L, oak::string{ "on_entity_collide", oak::frameAllocator });

			lua_newtable(L);
			oak::LuaPuper puper{ L, -2 };
			pup(puper, evt, {});

			oak::luah::call(L, 3, 0);
		});
		*/
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