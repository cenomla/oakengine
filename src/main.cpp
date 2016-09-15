#include <iostream>
#include <fstream>
#include <chrono>
#include <mutex>

#include "graphics/vulkan/renderer.h"

#include "log.h"
#include "events.h"
#include "engine.h"
#include "task.h"
#include "window.h"

using namespace std::chrono_literals;

int main(int argc, char** argv) {
	std::ofstream file{ "log" };
	oak::log::cout_stream.addStream(file);
	oak::log::cout_stream.addStream(std::cout);

	oak::log::cout << "oak engine version: 0.1.0" << std::endl;

	oak::Engine engine;
	engine.init();

	oak::Window window{ &engine };
	engine.addSystem("window", &window);

	
	oak::TaskManager &tm = engine.getTaskManager();
	
	tm.addTask({ [&engine](){
		std::this_thread::sleep_for(5s);
		engine.getEventManager().emitEvent(oak::QuitEvent{});
	}, oak::Task::BACKGROUND_BIT});





	oak::graphics::vulkan::Renderer renderer;
	renderer.init();

	engine.start();

	engine.destroy();

	

	return 0;
}