#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>

#include <graphics/opengl/gl_frame_renderer.h>
#include <graphics/opengl/gl_renderer.h>
#include <network/network_manager.h>
#include <log.h>
#include <events.h>
#include <engine.h>
#include <task.h>
#include <window.h>
#include <entity.h>
#include <script/binding.h>
#include <lua_manager.h>
#include <components.h>
#include <resource_manager.h>
#include <view_system.h>
#include <debugger.h>

#include "component_ext.h"
#include "event_ext.h"
#include "binding.h"
#include "systems.h"
#include "tile_system.h"
#include "light_renderer.h"

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
	oak::Debugger debugger{ engine };
	CollisionSystem collisionSystem{ engine };
	TileSystem tileSystem{ engine, 8, 16 };
	SpriteSystem spriteSystem{ engine };
	TextSystem textSystem{ engine };
	LightRenderer lightRenderer{ engine };
	oak::ComponentHandleStorage chstorage{ engine };

	//add the systems to the engine and therefore initilize them
	engine.addSystem(&luam);
	engine.addSystem(&resManager);
	engine.addSystem(&networkManager);
	engine.addSystem(&entityManager);
	engine.addSystem(&viewSystem);
	engine.addSystem(&debugger);
	engine.addSystem(&window);
	engine.addSystem(&frameRenderer);
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

	entityManager.initComponentPool<oak::TransformComponent>();
	entityManager.initComponentPool<oak::SpriteComponent>();
	entityManager.initComponentPool<oak::TextComponent>();
	entityManager.initComponentPool<oak::AABB2dComponent>();
	entityManager.initComponentPool<oak::PhysicsBody2dComponent>();
	entityManager.initComponentPool<OccluderComponent>();
	entityManager.initComponentPool<LightComponent>();

	//setup lua bindings
	lua_State *L = luam.getState();
	initBindings(L);
	//load main lua script
	oak::luah::loadScript(L, "res/scripts/main.lua");

	//first frame time
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	//physics accum for frame independent physics
	float accum = 0.0f;
	float fpsCount = 0.0f;
	float fpsAccum = 0.0f;

	//main game loop
	while (engine.isRunning()) {
		//set debug vars
		oak::debugVars.dt = dt.count();
		fpsAccum += dt.count();
		fpsCount ++;
		if (fpsAccum > 1.0f) {
			fpsAccum -= 1.0f;
			oak::debugVars.fps = fpsCount;
			fpsCount = 0.0f;
		}

		//emit update event in scripts (game logic)
		oak::luah::getGlobal(L, oak::string{ "oak.es.emit_event", oak::frameAllocator });
		oak::luah::getGlobal(L, oak::string{ "oak.es", oak::frameAllocator });
		oak::luah::pushValue(L, oak::string{ "on_update", oak::frameAllocator });
		oak::luah::pushValue(L, dt.count());
		oak::luah::call(L, 3, 0);

		oak::luah::getGlobal(L, oak::string{ "oak.es.process_events", oak::frameAllocator });
		oak::luah::getGlobal(L, oak::string{ "oak.es", oak::frameAllocator });
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
		//clear frame allocator
		oak::MemoryManager::inst().getFrameAllocator().clear();
	}

	//clean up
	engine.destroy();

	return 0;
}