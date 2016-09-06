#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

#include "event_queue.h"
#include "engine.h"
#include "task.h"


using namespace std::chrono_literals;

struct TestTask : public oak::Task {

	TestTask() : Task{ 0 } {}

	void run() {
		std::cout << "running" << std::endl;
		std::this_thread::sleep_for(2s);
	}

};

int main(int argc, char** argv) {
	std::cout << "oak engine version: 0.1.0" << std::endl;

	oak::Engine engine;

	engine.init();

	std::unique_ptr<TestTask> tt{ new TestTask{} };

	engine.getTaskManager().addTask(tt.get());

	engine.start();

	engine.destroy();

	

	return 0;
}