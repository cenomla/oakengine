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
	oak::ViewSystem viewSystem{ engine };
	CollisionSystem collisionSystem{ engine };
	TileSystem tileSystem{ engine, 8, 16 };
	SpriteSystem spriteSystem{ engine };
	TextSystem textSystem{ engine };

	//add the systems to the engine and therefore initilize them
	engine.addSystem(&resManager);
	engine.addSystem(&networkManager);
	engine.addSystem(&entityManager);
	engine.addSystem(&viewSystem);
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
	engine.getEventManager().add<oak::KeyEvent>([L](oak::KeyEvent evt) {
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "key_press");
		lua_newtable(L);
		oak::LuaPuper puper{ L, -2 };
		pup(puper, evt, {});
		oak::luah::call(L, 3, 0);
	});

	engine.getEventManager().add<oak::ButtonEvent>([L](oak::ButtonEvent evt){
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "button_press");
		lua_newtable(L);
		oak::LuaPuper puper{ L, -2 };
		pup(puper, evt, {});
		oak::luah::call(L, 3, 0);
	});
	
	engine.getEventManager().add<oak::MouseMoveEvent>([L](oak::MouseMoveEvent evt) {
		oak::luah::getGlobal(L, "oak.es.emit_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "mouse_move");
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
		oak::luah::pushValue(L, "entity_collide");

		lua_newtable(L);
		oak::LuaPuper puper{ L, -2 };
		pup(puper, evt, {});

		oak::luah::call(L, 3, 0);
	});

	engine.getEventManager().add<TileCollisionEvent>([L](TileCollisionEvent evt) {
		oak::luah::getGlobal(L, "oak.es.send_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "tile_collide");
		
		lua_newtable(L);
		oak::LuaPuper puper{ L, -2 };
		pup(puper, evt, {});

		oak::luah::call(L, 3, 0);
	});

	//register components with the entity manager
	entityManager.addComponentHandle<oak::TransformComponent>("transform");
	entityManager.addComponentHandle<oak::SpriteComponent>("sprite");
	entityManager.addComponentHandle<oak::TextComponent>("text");
	entityManager.addComponentHandle<oak::AABB2dComponent>("aabb_2d");
	entityManager.addComponentHandle<oak::PhysicsBody2dComponent>("physics_body_2d");

	//create resources
	//shaders
	auto &shd_pass = resManager.add<oak::graphics::GLShader>("shd_pass");
	shd_pass.create("core/graphics/shaders/pass2d/opengl.vert", "core/graphics/shaders/pass2d/opengl.frag");
	shd_pass.bindBlockIndex("MatrixBlock", 0);

	auto &shd_font = resManager.add<oak::graphics::GLShader>("shd_font");
	shd_font.create("core/graphics/shaders/font/opengl.vert", "core/graphics/shaders/font/opengl.frag");
	shd_font.bindBlockIndex("MatrixBlock", 0);

	//textures
	auto &tex_entityAtlas = resManager.add<oak::graphics::GLTextureAtlas>("tex_entityAtlas", GLenum{ GL_TEXTURE_2D });
	tex_entityAtlas.addTexture("res/textures/character.png");
	tex_entityAtlas.addTexture("res/textures/block.png");
	tex_entityAtlas.bake(512, 512);

	auto &tex_guiAtlas = resManager.add<oak::graphics::GLTextureAtlas>("tex_guiAtlas", GLenum{ GL_TEXTURE_2D });
	tex_guiAtlas.addTexture("res/textures/button.png");
	tex_guiAtlas.addTexture("res/textures/tile_editor.png");
	tex_guiAtlas.bake(512, 512);

	auto &tex_tiles = resManager.add<oak::graphics::GLTexture>("tex_tiles", GLenum{ GL_TEXTURE_2D });
	tex_tiles.create("res/textures/tiles.png");

	auto &tex_font = resManager.add<oak::graphics::GLTexture>("tex_font", GLenum{ GL_TEXTURE_2D }, GLenum{ GL_LINEAR });
	tex_font.create("res/fonts/dejavu_sans/atlas.png");

	//materials
	auto &mat_entity = resManager.add<oak::graphics::GLMaterial>("mat_entity", std::hash<std::string>{}("mat_entity"), &shd_pass, &tex_entityAtlas);
	auto &mat_gui = resManager.add<oak::graphics::GLMaterial>("mat_gui", std::hash<std::string>{}("mat_gui"), &shd_pass, &tex_guiAtlas);
	auto &mat_tiles = resManager.add<oak::graphics::GLMaterial>("mat_tiles", std::hash<std::string>{}("mat_tiles"), &shd_pass, &tex_tiles);
	auto &mat_font = resManager.add<oak::graphics::GLMaterial>("mat_font", std::hash<std::string>{}("mat_font"), &shd_font, &tex_font, 
		[](const oak::graphics::GLMaterial& mat) {
			mat.shader->setUniform1f("text_width", 0.5f);
			mat.shader->setUniform1f("text_edge", 0.1f);
			mat.shader->setVector3f("text_color", glm::vec3{ 1.0f });
			mat.shader->setUniform1f("border_width", 0.6f);
			mat.shader->setUniform1f("border_edge", 0.2f);
			mat.shader->setVector3f("border_color", glm::vec3{ 0.0f });
		});
	
	//fonts
	auto &fnt_dejavu = resManager.add<oak::graphics::Font>("fnt_dejavu", mat_font.id);
	fnt_dejavu.create("res/fonts/dejavu_sans/glyphs.fnt");

	//sprites
	resManager.add<oak::graphics::Sprite>("spr_player", mat_entity.id, 8.0f, 8.0f, 16.0f, 16.0f, tex_entityAtlas.getTextureRegion("res/textures/character.png"));
	resManager.add<oak::graphics::Sprite>("spr_block", mat_entity.id, 64.0f, 16.0f, 128.0f, 32.0f, tex_entityAtlas.getTextureRegion("res/textures/block.png"));
	resManager.add<oak::graphics::Sprite>("spr_button", mat_gui.id, 0.0f, 0.0f, 48.0f, 48.0f, tex_guiAtlas.getTextureRegion("res/textures/button.png"), 2);
	resManager.add<oak::graphics::Sprite>("spr_tile_editor", mat_gui.id, 0.0f, 0.0f, 256.0f, 512.0f, tex_guiAtlas.getTextureRegion("res/textures/tile_editor.png"));

	//strings
	resManager.add<std::string>("txt_play", "Start Game");
	resManager.add<std::string>("txt_load", "Load Game");
	resManager.add<std::string>("txt_options", "Options");
	resManager.add<std::string>("txt_quit", "Quit Game");

	//setup views
	viewSystem.defineView(0, { 0 });
	viewSystem.defineView(1, { 1 });
	viewSystem.setView(0, oak::View{ 128, 0, 1280, 720 });
	viewSystem.setView(1, oak::View{ 0, 0, 1280, 720 });

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