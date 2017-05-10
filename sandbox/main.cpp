#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>

#include <graphics/opengl/gl_renderer.h>
#include <log.h>
#include <system_manager.h>
#include <resource_manager.h>
#include <event_manager.h>
#include <window_system.h>
#include <view_system.h>
#include <resource.h>
#include <component_storage.h>
#include <components.h>
#include <entity.h>
#include <input_events.h>
#include <scene_events.h>

#include "component_ext.h"
#include "systems.h"
#include "tile_system.h"
#include "light_renderer.h"

struct stdoutstream : public oak::log::Stream {
	void write(const void *source, size_t size) override {
		fwrite(source, 1, size, stdout);
	}
};

struct logfilestream : public oak::log::Stream {
	void open() {
		if (file == nullptr) {
			file = fopen("log", "w");
		}
	}

	void close() {
		if (file != nullptr) {
			fclose(file);
			file = nullptr;
		}
	}

	void write(const void *source, size_t size) override {
		if (file != nullptr) {
			fwrite(source, 1, size, file);
			fflush(file);
		}
	}

	FILE *file = nullptr;
};

bool isRunning = false;

int main(int argc, char** argv) {
	//setup log
	stdoutstream sos;
	logfilestream lfs;
	lfs.open();
	oak::log::cout.addStream(&sos);
	oak::log::cout.addStream(&lfs);
	oak::log::cwarn.addStream(&sos);
	oak::log::cwarn.addStream(&lfs);
	oak::log::cerr.addStream(&sos);
	oak::log::cerr.addStream(&lfs);

	//init engine managers
	oak::SystemManager sysManager;
	oak::EventManager evtManager;
	oak::ResourceManager resManager;

	//add all events
	evtManager.addQueue<oak::EntityCreateEvent>();
	evtManager.addQueue<oak::EntityDestroyEvent>();
	evtManager.addQueue<oak::EntityActivateEvent>();
	evtManager.addQueue<oak::EntityDeactivateEvent>();
	evtManager.addQueue<oak::WindowCreateEvent>();
	evtManager.addQueue<oak::WindowCloseEvent>();
	evtManager.addQueue<oak::WindowResizeEvent>();
	evtManager.addQueue<oak::KeyEvent>();
	evtManager.addQueue<oak::ButtonEvent>();
	evtManager.addQueue<oak::CursorEvent>();
	evtManager.addQueue<oak::TextEvent>();

	//create the scene
	oak::Scene scene;

	//create all the systems
	oak::WindowSystem winSystem{ "oak engine sandbox" };
	oak::graphics::GLRenderer renderer;
	oak::ViewSystem viewSystem;
	TileSystem tileSystem{ 8, 16 };
	SpriteSystem spriteSystem{ scene };
	TextSystem textSystem{ scene };
	LightRenderer lightRenderer{ scene };
	//add them to the system manager
	sysManager.addSystem(winSystem, "window_system");
	sysManager.addSystem(renderer, "renderer");
	sysManager.addSystem(viewSystem, "view_system");
	sysManager.addSystem(tileSystem, "tile_system");
	sysManager.addSystem(spriteSystem, "sprite_system");
	sysManager.addSystem(textSystem, "text_system");
	sysManager.addSystem(lightRenderer, "light_renderer");
	//component type handle storage
	oak::ComponentHandleStorage chs;

	//create component type handles
	chs.addHandle<oak::TransformComponent>("transform");
	chs.addHandle<oak::SpriteComponent>("sprite");
	chs.addHandle<oak::TextComponent>("text");
	chs.addHandle<oak::AABB2dComponent>("aabb2d");
	chs.addHandle<OccluderComponent>("occluder");
	chs.addHandle<LightComponent>("light");

	//create component storage
	oak::ComponentStorage transformStorage{ &chs.getHandle<oak::TransformComponent>() };
	oak::ComponentStorage spriteStorage{ &chs.getHandle<oak::SpriteComponent>() };
	oak::ComponentStorage textStorage{ &chs.getHandle<oak::TextComponent>() };
	oak::ComponentStorage aabbStorage{ &chs.getHandle<oak::AABB2dComponent>() };
	oak::ComponentStorage occluderStorage{ &chs.getHandle<OccluderComponent>() };
	oak::ComponentStorage lightStorage{ &chs.getHandle<LightComponent>() };

	//add component storage to scene
	scene.addComponentStorage(chs.getId("transform"), transformStorage);
	scene.addComponentStorage(chs.getId("sprite"), spriteStorage);
	scene.addComponentStorage(chs.getId("text"), textStorage);
	scene.addComponentStorage(chs.getId("aabb2d"), aabbStorage);
	scene.addComponentStorage(chs.getId("occluder"), occluderStorage);
	scene.addComponentStorage(chs.getId("light"), lightStorage);

	auto& shd_pass = resManager.add<oak::graphics::GLShader>("shd_pass");
	auto& tex_player = resManager.add<oak::graphics::GLTexture>("tex_player", GLuint(GL_TEXTURE_2D));
	auto& mat_entity = resManager.add<oak::graphics::GLMaterial>("mat_entity", std::hash<oak::string>{}("mat_entity"), &shd_pass, &tex_player);
	auto& spr_player = resManager.add<oak::graphics::Sprite>("spr_player", mat_entity.id, 0.0f, 0.0f, 4.0f, 4.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	shd_pass.create("core/graphics/shaders/pass2d/opengl.vert", "core/graphics/shaders/pass2d/opengl.frag");
	tex_player.create("sandbox/res/textures/character.png");

	viewSystem.defineView(0, { 0 });
	viewSystem.setView(0, oak::View{ 0, 0, 1280, 720 });

	oak::EntityId id;
	oak::Entity entity{ id, &scene };
	for (size_t i = 0; i < 64000; i++) {
		id = scene.createEntity();
		entity = { id, &scene };
		entity.addComponent<oak::TransformComponent>(glm::vec3{ (i % 320) * 4.0f, (i / 320) * 4.0f, 0.0f }, 1.0f, glm::vec3{ 0.0f }, 0.0f);
		entity.addComponent<oak::SpriteComponent>(std::hash<oak::string>{}("spr_player"), glm::vec2{ 1.0f }, 0, 0, 0u);
		scene.activateEntity(id);
	}

	//first frame time
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> dt;

	//main game loop
	isRunning = true;
	while (isRunning) {
		//create / destroy / activate / deactivate entities 
		scene.update();

		/*
		//update physics
		accum += dt.count();
		while (accum >= 1.0f/60.0f) {
			//physicsSystem.update(1.0f/60.0f);
			accum -= 1.0f/60.0f; 
		}
		*/

		spriteSystem.run();
		textSystem.run();
		tileSystem.run();
		lightRenderer.run();

		renderer.run();

		winSystem.run();

		//check for exit
		for (const auto& event : evtManager.getQueue<oak::WindowCloseEvent>()) {
			isRunning = false;
			break;
		}

		//update the delta time
		std::chrono::high_resolution_clock::time_point currentFrame = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrame - lastFrame);
		lastFrame = currentFrame;
		
		//clear all event queues
		evtManager.clear();
		//clear frame allocator
		oak::oalloc_frame.clear();
	}

	//clean up
	scene.reset();

	lfs.close();

	return 0;
}
