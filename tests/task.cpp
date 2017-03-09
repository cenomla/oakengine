#include <iostream>

#include <engine.h>
#include <task_manager.h>
#include <worker.h>
#include <task.h>

using namespace oak;

std::mutex coutMutex;

void workFun(const oak::string &str) {
	std::lock_guard<std::mutex> lock{ coutMutex };
	std::cout << str << std::endl;
}

int main(int argc, char** argv) {

	Engine engine;
	engine.init();

	int i = 0;
	
	while (engine.isRunning()) {
		

		TaskId task1 = engine.getTaskManager().createTask([](){ workFun("task 1"); }, 0);
		TaskId task2 = engine.getTaskManager().createTask([](){ workFun("task 2"); }, 0);
		TaskId task3 = engine.getTaskManager().createTask([](){ workFun("task 3"); }, 0);
		TaskId task4 = engine.getTaskManager().createTask([](){ workFun("task 4"); }, 1);
		TaskId task5 = engine.getTaskManager().createTask([](){ workFun("task 5"); }, 1);
		
		engine.getTaskManager().depends(task3, task2);
		engine.getTaskManager().addChild(task2, task4);
		engine.getTaskManager().addChild(task2, task5);

		engine.getTaskManager().swap();


		engine.getTaskManager().kick();

		engine.getTaskManager().wait();

		workFun(oak::string{ "eof" } + std::to_string(i).c_str());
		i++;

		if (i > 128000) {
			engine.getEventManager().emitEvent(QuitEvent{});
		}

	}



	engine.destroy();


	return 0;

}