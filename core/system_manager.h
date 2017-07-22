#pragma once

#include "oak_assert.h"

#include "util/type_id.h"
#include "container.h"
#include "system.h"

namespace oak {

	namespace detail {
		struct BaseSystem {};
	}

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

		template<class TSystem>
		void addSystem(TSystem *system, const oak::string& name) {
			size_t tid = util::type_id<detail::BaseSystem, TSystem>::id;
			if (tid >= systems_.size()) {
				systems_.resize(tid + 1);
				names_.resize(tid + 1);
			}
			systems_[tid] = system;
			names_[tid] = name;
			system->init();
		}

		template<class TSystem>
		void removeSystem() {
			size_t tid = util::type_id<detail::BaseSystem, TSystem>::id;
			systems_[tid]->terminate();
			systems_[tid] = nullptr;
			names_[tid].clear();
		}

		template<class TSystem>
		TSystem& getSystem() {
			size_t tid = util::type_id<detail::BaseSystem, TSystem>::id;
			return *static_cast<TSystem*>(systems_[tid]);
		}

		template<class TSystem>
		oak::string getSystemName() {
			size_t tid = util::type_id<detail::BaseSystem, TSystem>::id;
			return names_[tid];
		}

		template<class TSystem>
		bool hasSystem() {
			size_t tid = util::type_id<detail::BaseSystem, TSystem>::id;
			return (tid < systems_.size() && systems_[tid]);
		}

		void clear();

	private:
		oak::vector<System*> systems_;
		oak::vector<oak::string> names_;
	};

}
