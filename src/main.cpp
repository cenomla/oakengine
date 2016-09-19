#include <iostream>
#include <streambuf>
#include <fstream>
#include <chrono>
#include <mutex>
#include <cstring>

#include <GLFW/glfw3.h>

#define OAK_USE_GL
#define OAK_USE_VULKAN
#include "graphics/renderer.h"

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
	std::ofstream file{ "log" };
	std::ostream log{ std::cout.rdbuf(&oak::log::cout_stream) };
	oak::log::cout_stream.addStream(file);
	oak::log::cout_stream.addStream(log);

	oak::log::cout << "oak engine version: 0.1.0" << std::endl;

	oak::Engine engine;
	engine.init();

	oak::Window window{ &engine, oak::Window::USE_VULKAN };
	engine.addSystem("window", &window);

	KeyListener listener;
	engine.getEventManager().add<oak::KeyEvent>(&listener);

	oak::graphics::Renderer<oak::graphics::VulkanApi> renderer;
	renderer.init(window.getWindowHandle());

	engine.start();

	renderer.destroy();
	engine.destroy();

	

	return 0;
}