#include <iostream>
#include <streambuf>
#include <fstream>
#include <chrono>
#include <mutex>
#include <cstring>

#define OAK_USE_GL
#define OAK_USE_VULKAN
#include "graphics/renderer.h"

#include <GLFW/glfw3.h>

#include "sprite_renderer.h"
#include "log.h"
#include "events.h"
#include "engine.h"
#include "task.h"
#include "window.h"

using namespace std::chrono_literals;

class TSDS : public oak::System {
public:
	TSDS(oak::Engine *engine) : oak::System{ engine }, 
		sprite1_{ 0, 16.0f, 16.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		sprite2_{ 1, 16.0f, 16.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f }
		 {}

	void init() {
		engine_->getTaskManager().addTask( oak::Task{
			[this]() {
				update();
			}, oak::Task::LOOP_BIT
		});
	}

	void update() {
		oak::SpriteRenderer *renderer = reinterpret_cast<oak::SpriteRenderer*>(engine_->getSystem("renderer"));
		//draw a bunch o sprites
		for (int i = 0; i < 1280/16; i++) {
			for (int j = 0; j < 720/16; j++) {
				renderer->addSprite({i * 16, j * 16, 1.0f}, 0, 0, 1.0f, 1.3f, i % 2 == 1 ? &sprite1_ : &sprite2_);
			}
		}

	}

private:
	oak::graphics::Sprite sprite1_;
	oak::graphics::Sprite sprite2_;
};

struct KeyListener {
	void operator()(const oak::KeyEvent &evt) {
		const char* keyName = glfwGetKeyName(evt.key, evt.scancode);
		std::cout << "recieved key: " << (keyName == NULL ? "UNKNOWN" : keyName) << std::endl;
	}
};

int main(int argc, char** argv) {
	//setup log
	std::ofstream file{ "log" };
	std::ostream log{ std::cout.rdbuf(&oak::log::cout_stream) };
	oak::log::cout_stream.addStream(file);
	oak::log::cout_stream.addStream(log);

	//create the engine
	oak::Engine engine;
	engine.init();

	//create and add the window system
	oak::Window window{ &engine, oak::Window::GL_CONTEXT };
	oak::graphics::Renderer<oak::graphics::OpenglApi> renderer;
	TSDS tsds{ &engine };
	oak::SpriteRenderer spriteRenderer{ &engine };
	
	//add a test key listener
	KeyListener listener;
	engine.getEventManager().add<oak::KeyEvent>(&listener);

	//add the systems to the engine and therefore initilize them
	engine.addSystem("window", &window);
	renderer.init(window.getWindowHandle());
	engine.addSystem("tsds", &tsds);
	engine.addSystem("renderer", &spriteRenderer);
	engine.getTaskManager().addTask( oak::Task{
		[&renderer]() {
			renderer.update();
		}, oak::Task::LOOP_BIT
	});

	//add the resize event to the window
	engine.getEventManager().add<oak::ResizeEvent>(&renderer);
	//start ur engines
	engine.start();

	renderer.destroy();
	engine.destroy();

	

	return 0;
}