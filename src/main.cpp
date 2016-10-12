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

private:
	oak::graphics::Sprite sprite1_;
	oak::graphics::Sprite sprite2_;
};

struct KeyListener {
	void operator()(const oak::KeyEvent &evt) const {
		const char* keyName = glfwGetKeyName(evt.key, evt.scancode);
		std::cout << "recieved key: " << (keyName == NULL ? "UNKNOWN" : keyName) << std::endl;
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

int main(int argc, char** argv) {
	//setup log
	std::ofstream file{ "log" };
	//std::ostream log{ std::cout.rdbuf(&oak::log::cout_stream) };
	oak::log::cout_stream.addStream(file);
	oak::log::cout_stream.addStream(std::cout);

	//create the engine
	oak::Engine engine;
	engine.init();

	//create and add the window system
	Timer timer{ engine };
	oak::Window window{ engine, oak::Window::GL_CONTEXT };
	oak::graphics::GLFrameRenderer frameRenderer{ engine };
	TSDS tsds{ engine };
	oak::graphics::GLSpriteRenderer spriteRenderer{ engine };
	
	//add a test key listener
	KeyListener listener;
	engine.getEventManager().add<oak::KeyEvent>(std::cref(listener));

	engine.getTaskManager().addTask({ [](){ 
		std::this_thread::sleep_for(std::chrono::seconds{1});
		std::cout << "this hour has 22 minutes" << std::endl;
	 }, oak::Task::MULTI_THREAD_BIT | oak::Task::LOOP_BIT });

	//add the systems to the engine and therefore initilize them
	engine.addSystem("timer", &timer);
	engine.addSystem("window", &window);
	engine.addSystem("tsds", &tsds);
	engine.addSystem("sprite_renderer", &spriteRenderer);
	engine.addSystem("frame_renderer", &frameRenderer);

	//start ur engines
	engine.start();

	engine.destroy();

	

	return 0;
}