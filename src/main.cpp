#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

#include <glad/glad.h>

#include "event_queue.h"
#include "events.h"
#include "engine.h"
#include "task.h"
#include "window.h"

using namespace std::chrono_literals;

std::mutex coutMutex;

int main(int argc, char** argv) {
	std::cout << "oak engine version: 0.1.0" << std::endl;

	oak::Engine engine;

	engine.init();

	

	oak::TaskManager &tm = engine.getTaskManager();

	tm.addTask(oak::Task{ [](){
		glViewport(0, 0, 1280, 720);
		glClearColor(0.3f, 0.2f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}, oak::Task::LOOP_BIT });

	oak::Window window{ engine };
	window.init();

	tm.addTask(oak::Task{ [&window](){
		window.update();
	}, oak::Task::LOOP_BIT });

	tm.addTask({ [&engine](){
		std::this_thread::sleep_for(5s);
		engine.getEventManager().emitEvent(oak::TaskExitEvent{});
	}, oak::Task::BACKGROUND_BIT});

	engine.start();

	engine.destroy();

	

	return 0;
}