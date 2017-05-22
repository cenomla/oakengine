#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>

#include <graphics/opengl/gl_renderer.h>
#include <graphics/opengl/gl_texture_atlas.h>
#include <log.h>
#include <system_manager.h>
#include <resource_manager.h>
#include <event_manager.h>
#include <input_manager.h>
#include <window_system.h>
#include <view_system.h>
#include <resource.h>
#include <component_storage.h>
#include <core_components.h>
#include <components.h>
#include <input_events.h>
#include <scene_events.h>
#include <update_events.h>
#include <input.h>
#include <prefab.h>

#include "component_ext.h"
#include "systems.h"
#include "tile_system.h"
#include "light_renderer.h"

class MovementSystem : public oak::System {
public:
	MovementSystem(oak::Scene& scene) : scene_{ &scene } {}

	void init() override {
		cache_.requireComponent<oak::TransformComponent>();
		cache_.requireComponent<oak::PrefabComponent>();
		cache_.requirePrefab(std::hash<oak::string>{}("player"));
	}

	void run() override {
		cache_.update(*scene_);

		auto& ts = oak::getComponentStorage<oak::TransformComponent>(*scene_);

		for (const auto& entity : cache_.entities()) {
			auto& tc = oak::getComponent<oak::TransformComponent>(ts, entity);
			
			if (oak::InputManager::inst().getAction("move_up")) {
				tc.position.y -= 4.0f;
			}
			if (oak::InputManager::inst().getAction("move_down")) {
				tc.position.y += 4.0f;
			}
			if (oak::InputManager::inst().getAction("move_left")) {
				tc.position.x -= 4.0f;
			}
			if (oak::InputManager::inst().getAction("move_right")) {
				tc.position.x += 4.0f;
			}
		}
	}

private:
	oak::EntityCache cache_;
	oak::Scene *scene_;
};

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
	oak::ComponentTypeManager chs;
	oak::EventManager evtManager;
	oak::ResourceManager resManager;
	oak::InputManager inputManager;

	inputManager.bind("move_up", oak::key::w, true);
	inputManager.bind("move_down", oak::key::s, true);
	inputManager.bind("move_left", oak::key::a, true);
	inputManager.bind("move_right", oak::key::d, true);

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
	evtManager.addQueue<oak::TickEvent>();
	evtManager.addQueue<oak::SimulateEvent>();

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
	MovementSystem movementSystem{ scene };
	//add them to the system manager
	sysManager.addSystem(winSystem, "window_system");
	sysManager.addSystem(renderer, "renderer");
	sysManager.addSystem(viewSystem, "view_system");
	sysManager.addSystem(tileSystem, "tile_system");
	sysManager.addSystem(spriteSystem, "sprite_system");
	sysManager.addSystem(textSystem, "text_system");
	sysManager.addSystem(lightRenderer, "light_renderer");
	sysManager.addSystem(movementSystem, "movement_system");

	//create component type handles
	chs.addHandle<oak::EventComponent>("event");
	chs.addHandle<oak::PrefabComponent>("prefab");
	chs.addHandle<oak::TransformComponent>("transform");
	chs.addHandle<oak::SpriteComponent>("sprite");
	chs.addHandle<oak::TextComponent>("text");
	chs.addHandle<oak::AABB2dComponent>("aabb2d");
	chs.addHandle<OccluderComponent>("occluder");
	chs.addHandle<LightComponent>("light");

	//create component storage
	oak::ComponentStorage eventStorage{ "event" };
	oak::ComponentStorage prefabStorage{ "prefab" };
	oak::ComponentStorage transformStorage{ "transform" };
	oak::ComponentStorage spriteStorage{ "sprite" };
	oak::ComponentStorage textStorage{ "text" };
	oak::ComponentStorage aabbStorage{ "aabb2d" };
	oak::ComponentStorage occluderStorage{ "occluder" };
	oak::ComponentStorage lightStorage{ "light" };

	//add component storage to scene
	scene.addComponentStorage(eventStorage);
	scene.addComponentStorage(prefabStorage);
	scene.addComponentStorage(transformStorage);
	scene.addComponentStorage(spriteStorage);
	scene.addComponentStorage(textStorage);
	scene.addComponentStorage(aabbStorage);
	scene.addComponentStorage(occluderStorage);
	scene.addComponentStorage(lightStorage);

	auto& shd_pass = resManager.add<oak::graphics::GLShader>("shd_pass");
	shd_pass.create("core/graphics/shaders/pass2d/opengl.vert", "core/graphics/shaders/pass2d/opengl.frag");
	auto& atlas = resManager.add<oak::graphics::GLTextureAtlas>("atlas", GLuint{ GL_TEXTURE_2D });
	atlas.addTexture("sandbox/res/textures/character.png");
	atlas.addTexture("sandbox/res/textures/platform.png");
	atlas.bake(512, 512);
	auto& mat_entity = resManager.add<oak::graphics::GLMaterial>("mat_entity", std::hash<oak::string>{}("mat_entity"), &shd_pass, &atlas);
	resManager.add<oak::graphics::Sprite>("spr_player", mat_entity.id, 0.0f, 0.0f, 32.0f, 32.0f, atlas.getTextureRegion("sandbox/res/rextures/character.png"));
	resManager.add<oak::graphics::Sprite>("spr_platform", mat_entity.id, 0.0f, 0.0f, 64.0f, 16.0f, atlas.getTextureRegion("sandbox/res/textures/platform.png"));

	viewSystem.defineView(0, { 0 });
	viewSystem.setView(0, oak::View{ 0, 0, 1280, 720 });

	auto& pf_platform = resManager.add<oak::Prefab>("pf_platform", "platform", scene);
	pf_platform.addComponent<oak::TransformComponent>();
	pf_platform.addComponent<oak::SpriteComponent>(std::hash<oak::string>{}("spr_platform"), glm::vec2{ 1.0f }, 0, 0, 0u);

	auto& pf_player = resManager.add<oak::Prefab>("pf_player", "player", scene);
	pf_player.addComponent<oak::TransformComponent>();
	pf_player.addComponent<oak::SpriteComponent>(std::hash<oak::string>{}("spr_player"), glm::vec2{ 1.0f }, 0, 0, 0u);


	oak::EntityId entity = pf_platform.createInstance();
	oak::getComponent<oak::TransformComponent>(scene, entity).position = glm::vec3{ 128.0, 512.0f, 0.0f };
	scene.activateEntity(entity);
	entity = pf_player.createInstance();
	scene.activateEntity(entity);

	//first frame time
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> dt;

	//main game loop
	isRunning = true;
	while (isRunning) {
		//refresh window, poll input
		winSystem.run();
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

		movementSystem.run();
		spriteSystem.run();
		textSystem.run();
		tileSystem.run();
		lightRenderer.run();

		renderer.run();

		//check for exit
		if (!evtManager.getQueue<oak::WindowCloseEvent>().empty()) {
			isRunning = false; 
		}

		//update the delta time
		std::chrono::high_resolution_clock::time_point currentFrame = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrame - lastFrame);
		lastFrame = currentFrame;
		
		//do engine things
		inputManager.update();
		evtManager.clear();
		oak::oalloc_frame.clear();
	}

	//clean up
	scene.reset();

	lfs.close();

	return 0;
}
