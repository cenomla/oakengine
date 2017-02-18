#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>

#include <graphics/opengl/gl_texture_atlas.h>
#include <graphics/opengl/gl_frame_renderer.h>
#include <graphics/opengl/gl_renderer.h>
#include <graphics/vertex.h>
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
#include <script/binding.h>
#include <components.h>
#include <resource_manager.h>
#include <view_system.h>
#include <event_queue.h>

#include "component_extentions.h"
#include "binding.h"
#include "tile_system.h"
#include "light_renderer.h"
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
	oak::ViewSystem viewSystem{ engine };
	CollisionSystem collisionSystem{ engine };
	TileSystem tileSystem{ engine, 8, 16 };
	SpriteSystem spriteSystem{ engine };
	TextSystem textSystem{ engine };
	LightRenderer lightRenderer{ engine };
	oak::ComponentHandleStorage chstorage{ engine };

	//add the systems to the engine and therefore initilize them
	engine.addSystem(&resManager);
	engine.addSystem(&networkManager);
	engine.addSystem(&entityManager);
	engine.addSystem(&viewSystem);
	engine.addSystem(&window);
	engine.addSystem(&frameRenderer);
	engine.addSystem(&luam);
	engine.addSystem(&collisionSystem);
	engine.addSystem(&spriteSystem);
	engine.addSystem(&textSystem);
	engine.addSystem(&tileSystem);
	engine.addSystem(&entityRenderer);
	engine.addSystem(&lightRenderer);
	engine.addSystem(&chstorage);

	//register components with the entity manager
	chstorage.addHandle<oak::TransformComponent>("transform");
	chstorage.addHandle<oak::SpriteComponent>("sprite");
	chstorage.addHandle<oak::TextComponent>("text");
	chstorage.addHandle<oak::AABB2dComponent>("aabb2d");
	chstorage.addHandle<oak::PhysicsBody2dComponent>("physics_body_2d");
	chstorage.addHandle<OccluderComponent>("occluder");
	chstorage.addHandle<LightComponent>("light");

	//setup lua bindings
	lua_State *L = luam.getState();
	initBindings(L);
	//load main lua script
	oak::luah::loadScript(L, "res/scripts/main.lua");

	//first frame time
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	//physics accum for frame independent physics
	float accum = 0.0f;
	//main game loop
	while (engine.isRunning()) {

		//emit update event in scripts (game logic)
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "on_update");
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

		//sumbit sprites and text to the renderer
		spriteSystem.update();
		textSystem.update();
		//sumbit chunks to the renderer
		tileSystem.update();
		//update lights
		lightRenderer.update();
		//render entities
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