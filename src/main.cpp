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

#include "log.h"
#include "events.h"
#include "engine.h"
#include "task.h"
#include "window.h"

using namespace std::chrono_literals;

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
	oak::Window window{ &engine, 0 };
	
	//add a test key listener
	KeyListener listener;
	engine.getEventManager().add<oak::KeyEvent>(&listener);

	//create the renderer object on the stack
	oak::graphics::Renderer<oak::graphics::VulkanApi> renderer;

	//create the task to update the renderer
	engine.getTaskManager().addTask(oak::Task{ [&renderer](){
		renderer.update();
	}, oak::Task::LOOP_BIT});
	//add the window to the engine and therefore initilize it
	engine.addSystem("window", &window);
	//add the resize event to the window
	engine.getEventManager().add<oak::ResizeEvent>(&renderer);
	//init the renderer
	renderer.init(window.getWindowHandle());
	//start ur engines
	engine.start();

	renderer.destroy();
	engine.destroy();

	

	return 0;
}