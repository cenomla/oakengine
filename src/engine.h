#pragma once

#include <unordered_map>

#include "oak_assert.h"
#include "memory/memory_manager.h"
#include "event_manager.h"
#include "task_manager.h"

namespace oak {

	class System;

	class Engine {
	public:
		inline static Engine& inst() { oak_assert(INST != nullptr); return *INST; }

		Engine();
		~Engine();

		void init();
		void destroy();

		void start();

		void addSystem(const std::string &name, System *system);
		void removeSystem(const std::string &name);

		System* getSystem(const std::string &name);

		EventManager& getEventManager() { return eventManager_; }
		const EventManager& getEventManager() const { return eventManager_; }

		TaskManager& getTaskManager() { return taskManager_; }
		const TaskManager& getTaskManager() const { return taskManager_; }

	private:
		static Engine *INST;
		MemoryManager memoryManager_;
		EventManager eventManager_;
		TaskManager taskManager_;
		
		std::unordered_map<std::string, System*> systems_;
	};

}
