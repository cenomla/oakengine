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

using namespace std::chrono_literals;

class TSDS : public oak::System {
public:
	TSDS(oak::Engine &engine) : oak::System{ engine }, 
		sprite1_{ 0, 16.0f, 16.0f, 0.0f, 0.0f, 1.0f, 1.0f, 8.0f, 8.0f },
		sprite2_{ 1, 16.0f, 16.0f, 0.0f, 0.0f, 1.0f, 1.0f, 8.0f, 8.0f }
		 {}

	void init() {
		engine_.getTaskManager().addTask({
			[this]() {
				update();
			}, oak::Task::LOOP_BIT
		});
	}

	void update() {
		auto *renderer = reinterpret_cast<oak::graphics::GLSpriteRenderer*>(engine_.getSystem("sprite_renderer"));
		//draw a bunch o sprites
		float rot = 0;
		for (int i = 1; i < 1280/32; i++) {
			for (int j = 1; j < 720/32; j++) {
				renderer->addSprite({i * 32.0f, j * 32.0f, 1.0f}, 0, 0, 1.0f, rot, i % 2 == 1 ? &sprite1_ : &sprite2_);
				rot += 1.3f;
			}
		}

	}

	oak::graphics::Sprite sprite1_;
	oak::graphics::Sprite sprite2_;
};

struct KeyListener {
	void operator()(const oak::KeyEvent &evt) const {
		const char* keyName = glfwGetKeyName(evt.key, evt.scancode);
		std::cout << "recieved key: " << (keyName == nullptr ? "UNKNOWN" : keyName) << std::endl;
	}
};

struct Timer : public oak::System {
	using oak::System::System;

	void init() {
		lastTime = std::chrono::high_resolution_clock::now();

		engine_.getTaskManager().addTask({
			[this](){
				auto currentTime = std::chrono::high_resolution_clock::now();

				dt = std::chrono::duration_cast<decltype(dt)>(currentTime - lastTime);

				lastTime = currentTime;

				//std::cout << "dt: " << dt.count() << std::endl;
			}, oak::Task::LOOP_BIT
		});
	}

	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
	std::chrono::duration<float, std::milli> dt;
};

struct TransformComponent {
	float x, y;
};

struct SpriteComponent {

};

class PlayerSystem : public oak::System {
public:
	PlayerSystem(oak::Engine &engine, oak::EntityManager &manager) : oak::System{ engine }, manager_{ manager } {

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
			const auto& tc = entity.getComponent<TransformComponent>();
			const auto* sprite = entity.getComponent<oak::graphics::Sprite*>();
			auto *renderer = reinterpret_cast<oak::graphics::GLSpriteRenderer*>(engine_.getSystem("sprite_renderer"));
			renderer->addSprite({ tc.x, tc.y, -1.0f }, 0, 0, 1.0f, 0.0f, sprite);
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
	Timer timer{ engine };
	oak::Window window{ engine, oak::Window::GL_CONTEXT };
	oak::graphics::GLFrameRenderer frameRenderer{ engine };
	TSDS tsds{ engine };
	oak::graphics::GLSpriteRenderer spriteRenderer{ engine };
	
	//create the entity manager
	oak::EntityManager manager;
	manager.addComponentHandle<TransformComponent>();
	manager.addComponentHandle<oak::graphics::Sprite*>();
	engine.getTaskManager().addTask({ [&manager](){ manager.update(); }, oak::Task::LOOP_BIT });
	//create a test entity
	oak::Entity entity = manager.createEntity(5);
	entity.addComponent<TransformComponent>(128.0f, 128.0f);
	entity.addComponent<oak::graphics::Sprite*>(&tsds.sprite1_);
	entity.activate();

	//create the test system
	PlayerSystem ps{ engine, manager };


	//add a test key listener
	KeyListener listener;
	engine.getEventManager().add<oak::KeyEvent>(std::cref(listener));

	//add the systems to the engine and therefore initilize them
	engine.addSystem("timer", &timer);
	engine.addSystem("window", &window);
	engine.addSystem("tsds", &tsds);
	engine.addSystem("player_system", &ps);
	engine.addSystem("sprite_renderer", &spriteRenderer);
	engine.addSystem("frame_renderer", &frameRenderer);

	//start ur engines
	engine.start();

	//clean up resources
	engine.destroy();

	

	return 0;
}