#pragma once

#include <unordered_map>

#include "oak_assert.h"
#include "memory/memory_manager.h"
#include "event_manager.h"
#include "task_manager.h"
#include "system.h"
#include "log.h"

namespace oak {

	class Engine {
	public:
		inline static Engine& inst() { oak_assert(INST != nullptr); return *INST; }

		Engine();
		~Engine();

		void init();
		void destroy();

		template<typename T>
		void addSystem(T *sys) {
			size_t tid = util::type_id<System, T>::id;
			const auto it = systems_.find(tid);
			//system doesnt exist
			if (it == std::end(systems_)) {
				sys->init();
				systems_.insert(it, { tid, sys });
			}
		}

		template<typename T>
		void removeSystem() {
			size_t tid = util::type_id<System, T>::id;
			const auto it = systems_.find(tid);
			if (it != std::end(systems_)) {
				it->second->destroy();
				systems_.erase(it);
			}
		}

		void removeSystem(const std::string &name);

		template<typename T>
		T& getSystem() {
			size_t tid = util::type_id<System, T>::id;
			const auto it = systems_.find(tid);
			//system exists
			if (it != std::end(systems_)) {
				return *static_cast<T*>(it->second);
			} else {
				log::cout << "system does not exist" << std::endl;
				abort();
			}
		}

		System* getSystem(const std::string &name);

		inline EventManager& getEventManager() { return eventManager_; }
		inline const EventManager& getEventManager() const { return eventManager_; }

		inline TaskManager& getTaskManager() { return taskManager_; }
		inline const TaskManager& getTaskManager() const { return taskManager_; }

		inline void operator()(const QuitEvent&) { isRunning_ = false; }
		inline bool isRunning() const { return isRunning_; }

	private:
		static Engine *INST;
		MemoryManager memoryManager_;
		EventManager eventManager_;
		TaskManager taskManager_;
		
		std::unordered_map<size_t, System*> systems_;
		bool isRunning_;
	};

}
