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
#include <component_storage.h>
#include <components.h>
#include <entity.h>

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

	//create the scene
	oak::Scene scene;

	//create all the systems
	oak::WindowSystem winSystem{ "oak engine sandbox" };
	oak::graphics::GLRenderer renderer;
	oak::ViewSystem viewSystem;
	TileSystem tileSystem{ 8, 16 };
	SpriteSystem spriteSystem{ &scene };
	TextSystem textSystem{ &scene };
	LightRenderer lightRenderer{ &scene };
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

		//update the delta time
		std::chrono::high_resolution_clock::time_point currentFrame = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrame - lastFrame);
		lastFrame = currentFrame;
		//clear frame allocator
		oak::oalloc_frame.clear();
	}

	//clean up
	scene.terminate();

	lfs.close();

	return 0;
}