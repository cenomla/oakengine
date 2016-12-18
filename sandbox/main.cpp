#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>

#include <graphics/opengl/gl_frame_renderer.h>
#include <graphics/opengl/gl_renderer.h>
#include <graphics/font.h>
#include <network/network_manager.h>
#include <log.h>
#include <events.h>
#include <engine.h>
#include <task.h>
#include <window.h>
#include <entity.h>
#include <prefab.h>
#include <lua_manager.h>
#include <script/lua_puper.h>
#include <binding.h>
#include <components.h>
#include <resource_manager.h>
#include <event_queue.h>

#include "binding.h"
#include "tile_system.h"
#include "systems.h"
#include "events.h"

std::chrono::duration<float> dt;

int main(int argc, char** argv) {
	//setup log
	std::ofstream file{ "log" };
	//std::ostream log{ std::cout.rdbuf(&oak::log::cout_stream) };
	oak::log::cout_stream.addStream(file);
	oak::log::cout_stream.addStream(std::cout);

	//create the engine
	oak::Engine engine;
	engine.init();

	//create all the systems
	oak::Window window{ engine, oak::Window::GL_CONTEXT };
	oak::LuaManager luam{ engine };
	oak::graphics::GLFrameRenderer frameRenderer{ engine };
	oak::graphics::GLRenderer entityRenderer{ engine };
	oak::ResourceManager resManager{ engine };
	oak::network::NetworkManager networkManager{ engine };
	oak::EntityManager entityManager{ engine };
	CollisionSystem collisionSystem{ engine };
	TileSystem tileSystem{ engine, 8, 16 };
	SpriteSystem spriteSystem{ engine };
	TextSystem textSystem{ engine };

	//add the systems to the engine and therefore initilize them
	engine.addSystem(&resManager);
	engine.addSystem(&networkManager);
	engine.addSystem(&entityManager);
	engine.addSystem(&window);
	engine.addSystem(&frameRenderer);
	engine.addSystem(&luam);
	engine.addSystem(&collisionSystem);
	engine.addSystem(&tileSystem);
	engine.addSystem(&spriteSystem);
	engine.addSystem(&textSystem);
	engine.addSystem(&entityRenderer);

	//set up event listeners that push input events to lua
	lua_State *L = luam.getState();
	engine.getEventManager().add<oak::KeyEvent>([L](const oak::KeyEvent &evt) {
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "key_press");
		oak::luah::pushValue(L, evt.key);
		oak::luah::pushValue(L, evt.scancode);
		oak::luah::pushValue(L, evt.action);
		oak::luah::pushValue(L, evt.mods);
		oak::luah::call(L, 6, 0);
	});
	engine.getEventManager().add<oak::ButtonEvent>([L](const oak::ButtonEvent &evt){
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "button_press");
		oak::luah::pushValue(L, evt.button);
		oak::luah::pushValue(L, evt.action);
		oak::luah::pushValue(L, evt.mods);
		oak::luah::call(L, 5, 0);
	});
	engine.getEventManager().add<oak::MouseMoveEvent>([L](const oak::MouseMoveEvent &evt) {
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "mouse_move");
		oak::luah::pushValue(L, evt.x);
		oak::luah::pushValue(L, evt.y);
		oak::luah::call(L, 4, 0);
	});
	engine.getEventManager().add<oak::EntityCollisionEvent>([L](const oak::EntityCollisionEvent &evt) {
		oak::luah::getGlobal(L, "oak.es.send_message");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "entity_collide");

		oak::luah::getGlobal(L, "oak.es.get_entity");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, evt.entity.index());
		oak::luah::call(L, 2, 1);

		oak::luah::getGlobal(L, "oak.es.get_entity");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, evt.entity.index());
		oak::luah::call(L, 2, 1);

		lua_newtable(L);
		oak::LuaPuper puper{ L, -2 };
		glm::vec2 n = evt.normal;
		oak::util::pup(puper, n, {});
		oak::luah::pushValue(L, evt.depth);
		oak::luah::call(L, 6, 0);
	});
	engine.getEventManager().add<TileCollisionEvent>([L](const TileCollisionEvent &evt) {
		oak::luah::getGlobal(L, "oak.es.send_message");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "tile_collide");
		oak::luah::getGlobal(L, "oak.es.get_entity");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, evt.entity.index());
		oak::luah::call(L, 2, 1);
		lua_newtable(L);
		oak::LuaPuper puper{ L, -2 };
		Tile t = evt.tile;
		pup(puper, t, {});
		puper.setIndex(6);
		lua_newtable(L);
		glm::vec2 n = evt.normal;
		oak::util::pup(puper, n, {});
		oak::luah::pushValue(L, evt.depth);
		oak::luah::call(L, 6, 0);
	});

	//register components with the entity manager
	entityManager.addComponentHandle<oak::TransformComponent>();
	entityManager.addComponentHandle<oak::SpriteComponent>();
	entityManager.addComponentHandle<oak::TextComponent>();
	entityManager.addComponentHandle<oak::AABB2dComponent>();
	entityManager.addComponentHandle<oak::PhysicsBody2dComponent>();

	//create resources
	auto &mat = resManager.add<oak::graphics::GLMaterial>("mat_player", std::hash<std::string>{}("mat_player"), oak::graphics::GLShader{}, oak::graphics::GLTexture{ GL_TEXTURE_2D });
	mat.create("core/graphics/shaders/pass2d", "res/textures/character");

	auto &mat0 = resManager.add<oak::graphics::GLMaterial>("mat_block", std::hash<std::string>{}("mat_block"), oak::graphics::GLShader{}, oak::graphics::GLTexture{ GL_TEXTURE_2D });
	mat0.create("core/graphics/shaders/pass2d", "res/textures/block");

	auto &mat_tiles = resManager.add<oak::graphics::GLMaterial>("mat_tiles", std::hash<std::string>{}("mat_tiles"), oak::graphics::GLShader{}, oak::graphics::GLTexture{ GL_TEXTURE_2D });
	mat_tiles.create("core/graphics/shaders/pass2d", "res/textures/tiles");

	auto &mat_font = resManager.add<oak::graphics::GLMaterial>("mat_font", std::hash<std::string>{}("mat_font"), oak::graphics::GLShader{}, oak::graphics::GLTexture{ GL_TEXTURE_2D });
	mat_font.create("core/graphics/shaders/pass2d", "res/fonts/dejavu_sans/atlas");
	
	auto &fnt_dejavu = resManager.add<oak::graphics::Font>("fnt_dejavu", mat_font.id);
	fnt_dejavu.create("res/fonts/dejavu_sans/glyphs.fnt");

	resManager.add<oak::graphics::Sprite>("spr_player", mat.id, 16.0f, 16.0f, 0.0f, 0.0f, 1.0f, 1.0f, 8.0f, 8.0f);
	resManager.add<oak::graphics::Sprite>("spr_block", mat0.id, 128.0f, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f, 64.0f, 16.0f);

	resManager.add<std::string>("txt_play", "Start Game");
	resManager.add<std::string>("txt_load", "Load Game");
	resManager.add<std::string>("txt_options", "Options");
	resManager.add<std::string>("txt_quit", "Quit Game");

	auto& prefab = resManager.add<oak::Prefab>("player", &entityManager);
	prefab.addComponent<oak::TransformComponent>(false, glm::vec3{ 216.0f, 128.0f, 0.0f }, 0.5f, glm::vec3{ 0.0f }, 0.0f);
	prefab.addComponent<oak::SpriteComponent>(true, std::hash<std::string>{}("spr_player"), 0, 0);
	prefab.addComponent<oak::AABB2dComponent>(true, glm::vec2{ 8.0f, 8.0f }, glm::vec2{ 0.0f, 0.0f });
	prefab.addComponent<oak::PhysicsBody2dComponent>(false, glm::vec2{ 0.0f }, glm::vec2{ 0.0f }, 0.025f * 0.2f, 0.4f, 0.2f, 0.1f);
	prefab.addComponent<oak::TextComponent>(true, std::hash<std::string>{}("fnt_dejavu"), std::hash<std::string>{}("txt_play"));

	auto& prefab0 = resManager.add<oak::Prefab>("block", &entityManager);
	prefab0.addComponent<oak::SpriteComponent>(true, std::hash<std::string>{}("spr_block"), 0, 0);
	prefab0.addComponent<oak::TransformComponent>(false, glm::vec3{ 256.0f, 512.0f, 0.0f }, 2.0f, glm::vec3{ 0.0f }, 0.0f);
	prefab0.addComponent<oak::AABB2dComponent>(true, glm::vec2{ 128.0f, 32.0f }, glm::vec2{ 0.0f, 0.0f });
	prefab0.addComponent<oak::PhysicsBody2dComponent>(false, glm::vec2{ 0.0f }, glm::vec2{ 0.0f }, 0.0f, 0.4f, 0.5f, 0.4f);

	auto& fab_button = resManager.add<oak::Prefab>("button", &entityManager);
	fab_button.addComponent<oak::TransformComponent>(false, glm::vec3{ 0.0f }, 1.0f, glm::vec3{ 0.0f }, 0.0f);
	//fab_button.addComponent<oak::AABB2dComponent>(true, glm::vec2{ 48.0f, 12.0f }, glm::vec2{ 0.0f, 0.0f });

	initBindings(L);
	//add the tile system to the oak global table
	lua_getglobal(L, "oak");
	oak::luah::pushValue(L, tileSystem);
	lua_setfield(L, -2, "ts");
	lua_pop(L, 1);

	//load main lua script
	oak::luah::loadScript(luam.getState(), "res/scripts/main.lua");

	//first frame time
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	//physics accum for frame independent physics
	float accum = 0.0f;
	//main game loop
	while (engine.isRunning()) {

		//emit update event in scripts (game logic)
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "update");
		oak::luah::pushValue(L, dt.count());
		oak::luah::call(L, 3, 0);

		oak::luah::getGlobal(L, "oak.es.process_events");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::call(L, 1, 0);

		//create / destroy / activate / deactivate entities 
		entityManager.update();

		collisionSystem.update();

		//update physics
		accum += dt.count();
		while (accum >= 1.0f/60.0f) {
			//physicsSystem.update(1.0f/60.0f);
			accum -= 1.0f/60.0f; 
		}

		//sumbit sprites and text to the entity renderer
		spriteSystem.update();
		textSystem.update();
		//render tiles
		tileSystem.render(mat_tiles);
		//render sprites
		entityRenderer.render();
		//swap buffers, clear buffer check for opengl errors
		frameRenderer.render();

		//poll input and check if the window should be closed
		window.update();

		//update the delta time
		std::chrono::high_resolution_clock::time_point currentFrame = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrame - lastFrame);
		lastFrame = currentFrame;
	}

	//clean up
	engine.destroy();

	return 0;
}