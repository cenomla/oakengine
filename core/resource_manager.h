#pragma once

#include "oak_assert.h"
#include "resource.h"
#include "util/type_id.h"
#include "container.h"
#include "log.h"

namespace oak {

	namespace detail {
		struct BaseResource {};
	}

	class ResourceManager {
	private:
		static ResourceManager *instance;
	public:
		static ResourceManager& inst() { 
			oak_assert(instance != nullptr);
			return *instance; 
		}

		ResourceManager();
		~ResourceManager();

		template<class T>
		void add(ResourceHandler<T> *handler) {
			size_t tid = util::type_id<detail::BaseResource, T>::id;

			if (resourceHandles_.size() <= tid) { 
				resourceHandles_.resize(tid+1);
			}
			if (resourceHandles_[tid] == nullptr) {
				resourceHandles_[tid] = handler;
			} else {
				log_print_warn("resouce handler already exists");
			}
		}

		template<class T>
		ResourceHandler<T>& get() {
			size_t tid = util::type_id<detail::BaseResource, T>::id;

			if (!(tid < resourceHandles_.size() && resourceHandles_[tid])) {
				log_print_err("resource handler does not exist");
				abort();
			}

			return *static_cast<ResourceHandler<T>*>(resourceHandles_[tid]);
		}

		template<class T>
		T* require(size_t id) {
			return get<T>().require(id);
		}

		template<class T>
		T& require(const oak::string &name) {
			size_t id = std::hash<oak::string>{}(name);
			auto r = require<T>(id);
			if (r) {
				return *r;
			}
			log_print_err("failed to find resource: %s", name.c_str());
			abort();
		}

	private:
		oak::vector<void*> resourceHandles_;
	};

}
