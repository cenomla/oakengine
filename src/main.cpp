#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

#include "event_queue.h"
#include "events.h"
#include "engine.h"
#include "task.h"


using namespace std::chrono_literals;

std::mutex coutMutex;

int main(int argc, char** argv) {
	std::cout << "oak engine version: 0.1.0" << std::endl;

	oak::Engine engine;

	engine.init();

	oak::TaskManager &tm = engine.getTaskManager();

	tm.addTask(oak::Task{ [](){
		{
			std::lock_guard<std::mutex> lock{ coutMutex };
			std::cout << "test task 1" << std::endl;
		}
		std::this_thread::sleep_for(1s);
	}, oak::Task::LOOP_BIT });

	tm.addTask(oak::Task{ [](){
		for (size_t i = 0; i < 4; i++) {
			{
				std::lock_guard<std::mutex> lock{ coutMutex };
				std::cout << "multi task: " << std::this_thread::get_id() << " | " << i << std::endl;
			}
			std::this_thread::sleep_for(200ms);
		}
	}, oak::Task::MULTI_THREAD_BIT | oak::Task::LOOP_BIT });

	tm.addTask(oak::Task{ [](){
		for (size_t i = 0; i < 200; i++) {
			{
				std::lock_guard<std::mutex> lock{ coutMutex };
				std::cout << "background task: " << std::this_thread::get_id() << " | " << i << std::endl;
			}
			std::this_thread::sleep_for(100ms);
		}
	}, oak::Task::BACKGROUND_BIT });

	tm.addTask({ [&engine](){
		engine.getEventManager().emitEvent(oak::TaskExitEvent{});
	}, 0});

	engine.start();

	engine.destroy();

	

	return 0;
}