#include <iostream>
#include <fstream>
#include <chrono>

#include "graphics/opengl/gl_frame_renderer.h"
#include "graphics/opengl/gl_sprite_renderer.h"
#include <GLFW/glfw3.h>

#include "log.h"
#include "events.h"
#include "engine.h"
#include "task.h"
#include "window.h"
#include "entity.h"
#include "prefab.h"
#include "lua_manager.h"
#include "components.h"
#include "resource_manager.h"

class SpriteSystem : public oak::System {
public:
	SpriteSystem(oak::Engine &engine) : oak::System{ engine, "sprite_system" } {

	}

	void init() override {
		cache_.requireComponent<oak::TransformComponent>();
		cache_.requireComponent<oak::SpriteComponent>();
		engine_.getSystem<oak::EntityManager>().addCache(&cache_);
		engine_.getEventManager().add<oak::KeyEvent>(std::ref(*this));
		engine_.getTaskManager().addTask({ [this](){ update(); }, oak::Task::LOOP_BIT });
	}

	void update() {
		for (const auto& entity : cache_.entities()) {
			const auto &tc = entity.getComponent<oak::TransformComponent>();
			const auto &sc = entity.getComponent<oak::SpriteComponent>();
			auto &renderer = engine_.getSystem<oak::graphics::GLSpriteRenderer>();
			renderer.addSprite(tc.position, sc.animFrameX, sc.animFrameY, tc.scale, tc.rotationAngle, sc.sprite);
		}
	}

	void operator()(const oak::KeyEvent &evt) {
		auto entity = cache_.entities()[0];
		auto &tc = entity.getComponent<oak::TransformComponent>();
		if (evt.action != GLFW_RELEASE) {
			if (evt.key == GLFW_KEY_W) {
				tc.position.y -= 4;
			}
			if (evt.key == GLFW_KEY_A) {
				tc.position.x -= 4;
			}
			if (evt.key == GLFW_KEY_S) {
				tc.position.y += 4;
			}
			if (evt.key == GLFW_KEY_D) {
				tc.position.x += 4;
			}
		}
	}
private:
	oak::EntityCache cache_;

};

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
	oak::LuaManager luam{ engine, "res/scripts/main.lua"};
	oak::graphics::GLFrameRenderer frameRenderer{ engine };
	oak::graphics::GLSpriteRenderer spriteRenderer{ engine };
	oak::ResourceManager resManager{ engine };
	oak::EntityManager entityManager{ engine };
	SpriteSystem spriteSystem{ engine };
	
	//add the systems to the engine and therefore initilize them
	engine.addSystem(&resManager);
	engine.addSystem(&entityManager);
	engine.addSystem(&window);
	engine.addSystem(&frameRenderer);
	engine.addSystem(&luam);
	engine.addSystem(&spriteSystem);
	engine.addSystem(&spriteRenderer);

	//init some test stuff
	entityManager.addComponentHandle<oak::TransformComponent>();
	entityManager.addComponentHandle<oak::SpriteComponent>();
	const auto& sprite = resManager.add<oak::graphics::Sprite>("spr_player", 0u, 16.0f, 16.0f, 0.0f, 0.0f, 1.0f, 1.0f, 8.0f, 8.0f);
	auto& prefab = resManager.add<oak::Prefab>("pre_player", &entityManager);
	prefab.addComponent<oak::TransformComponent>(glm::vec3{128.0f, 128.0f, 0.0f}, 1.0f, glm::vec3{0.0f}, 0.0f);
	prefab.addComponent<oak::SpriteComponent>(&sprite, 0, 0);

	//create a test entity
	oak::Entity entity = prefab.createInstance(0);
	entity.activate();

	//start ur engines
	engine.run();
	engine.destroy();

	return 0;
}