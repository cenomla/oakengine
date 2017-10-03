#pragma once

#include "oak_assert.h"

#include "util/type_id.h"
#include "container.h"
#include "system.h"

namespace oak {

	class SystemManager {
	private:
		static SystemManager *instance;
	public:
		static SystemManager& inst() { 
			oak_assert(instance != nullptr);
			return *instance;
		}

		SystemManager();
		~SystemManager();

		void addSystem(System *system, size_t id);
		void removeSystem(size_t id);

		System* getSystem(size_t id);
		bool hasSystem(size_t id);

		void clear();

	private:
		oak::unordered_map<size_t, System*> systems_;
	};

}
