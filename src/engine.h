#pragma once

#include <unordered_map>

#include "event_manager.h"
#include "task_manager.h"

namespace oak {

	class System;

	class Engine {
	public:
		inline static const Engine& inst() { return *INST; }

		Engine();
		~Engine();

		void init();
		void destroy();

		void start();

		void addSystem(const std::string &name, System *system);
		void removeSystem(const std::string &name);

		EventManager& getEventManager() { return eventManager_; }
		const EventManager& getEventManager() const { return eventManager_; }

		TaskManager& getTaskManager() { return taskManager_; }
		const TaskManager& getTaskManager() const { return taskManager_; }

	private:
		static Engine *INST;
		EventManager eventManager_;
		TaskManager taskManager_;
		
		std::unordered_map<std::string, System*> systems_;
	};

}
