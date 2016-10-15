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

struct TransformComponent {
	float x, y;
};

struct SpriteComponent {

};

class PlayerSystem : public oak::System {
public:
	PlayerSystem(oak::Engine &engine, oak::EntityManager &manager) : oak::System{ engine, "player_system" }, manager_{ manager } {

	}

	void init() override {
		cache_.requireComponent<TransformComponent>();
		cache_.requireComponent<oak::graphics::Sprite*>();
		manager_.addCache(&cache_);
		engine_.getEventManager().add<oak::KeyEvent>(std::ref(*this));
		engine_.getTaskManager().addTask({ [this](){ update(); }, oak::Task::LOOP_BIT });
	}

	void update() {
		for (const auto& entity : cache_.entities()) {
			const auto &tc = entity.getComponent<TransformComponent>();
			const auto *sprite = entity.getComponent<oak::graphics::Sprite*>();
			auto &renderer = engine_.getSystem<oak::graphics::GLSpriteRenderer>();
			renderer.addSprite({ tc.x, tc.y, -1.0f }, 0, 0, 1.0f, 0.0f, sprite);
		}
	}

	void operator()(const oak::KeyEvent &evt) {
		auto entity = cache_.entities()[0];
		auto &tc = entity.getComponent<TransformComponent>();
		if (evt.action != GLFW_RELEASE) {
			if (evt.key == GLFW_KEY_W) {
				tc.y -= 4;
			}
			if (evt.key == GLFW_KEY_A) {
				tc.x -= 4;
			}
			if (evt.key == GLFW_KEY_S) {
				tc.y += 4;
			}
			if (evt.key == GLFW_KEY_D) {
				tc.x += 4;
			}
		}
	}
private:
	oak::EntityManager &manager_;
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

	//create all the systems on the stack (why not)
	oak::Window window{ engine, oak::Window::GL_CONTEXT };
	oak::LuaManager luam{ engine, "res/scripts/main.lua"};
	oak::graphics::GLFrameRenderer frameRenderer{ engine };
	oak::graphics::GLSpriteRenderer spriteRenderer{ engine };
	
	//create the entity manager
	oak::EntityManager manager;
	manager.addComponentHandle<TransformComponent>();
	manager.addComponentHandle<oak::graphics::Sprite*>();
	engine.getTaskManager().addTask({ [&manager](){ manager.update(); }, oak::Task::LOOP_BIT });
	//create a test prefab
	oak::graphics::Sprite sprite{ 0, 16.0f, 16.0f, 0.0f, 0.0f, 1.0f, 1.0f, 8.0f, 8.0f };

	oak::Prefab prefab{ &manager };
	prefab.addComponent<TransformComponent>(128.0f, 128.0f);
	prefab.addComponent<oak::graphics::Sprite*>(&sprite);

	//create a test entity
	oak::Entity entity = prefab.createInstance(0);
	entity.activate();

	prefab.clear();

	//create the test system
	PlayerSystem ps{ engine, manager };

	//add the systems to the engine and therefore initilize them
	engine.addSystem(&window);
	engine.addSystem(&luam);
	engine.addSystem(&ps);
	engine.addSystem(&spriteRenderer);
	engine.addSystem(&frameRenderer);

	//start ur engines
	engine.start();

	//clean up resources
	engine.destroy();

	

	return 0;
}