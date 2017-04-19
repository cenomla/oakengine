#pragma once

#include "util/typeid.h"
#include "container.h"
#include "oak_alloc.h"
#include "system.h"
#include "log.h"

namespace oak {

	class ResourceListHandlerBase {
	public:
		virtual ~ResourceListHandlerBase() {}
	};

	template<class T>
	class ResourceListHandler : public ResourceListHandlerBase {
	public:
		template<class... TArgs>
		T* addResource(size_t id, TArgs&&... args) {
			auto it = resources_.insert_or_assign(id, T{ std::forward<TArgs>(args)... });
			return &it.first->second;
		}

		const T* getResource(size_t id) const {
			const auto it = resources_.find(id);
			if (it != std::end(resources_)) {
				return &it->second;
			} else {
				return nullptr;
			}
		}
	private:
		oak::unordered_map<size_t, T> resources_;
	};

	class ResourceManager : public System {
	public:
		ResourceManager(Engine &engine) : System(engine, "resource_manager") {}

		void destroy() override {
			for (auto block : resourceHandles_) {
				if (block.ptr != nullptr) {
					static_cast<ResourceListHandlerBase*>(block.ptr)->~ResourceListHandlerBase();
					proxyAllocator.deallocate(block.ptr, block.size);
				}
			}
		}

		template<class T, class... TArgs>
		T& add(const oak::string &name, TArgs&&... args) {
			size_t tid = util::type_id<Resource, T>::id;

			if (tid >= resourceHandles_.size()) { resourceHandles_.resize(tid+1); }
			if (resourceHandles_.at(tid).ptr == nullptr) { 
				auto ptr = static_cast<ResourceListHandler<T>*>(proxyAllocator.allocate(sizeof(ResourceListHandler<T>)));
				new (ptr) ResourceListHandler<T>{};
				resourceHandles_[tid] = { ptr, sizeof(ResourceListHandler<T>) }; 
			}

			auto plh = static_cast<ResourceListHandler<T>*>(resourceHandles_.at(tid).ptr);
			return *plh->addResource(std::hash<oak::string>{}(name), std::forward<TArgs>(args)...);
		}

		template<class T>
		const T* try_require(size_t id) {
			size_t tid = util::type_id<Resource, T>::id;
			
			if (tid < resourceHandles_.size() && resourceHandles_.at(tid).ptr != nullptr) {
				const auto plh = static_cast<ResourceListHandler<T>*>(resourceHandles_.at(tid).ptr);

				const auto pres = plh->getResource(id);
				if (pres != nullptr) {
					return pres;
				}
			}

			return nullptr;
		}

		template<class T>
		const T& require(size_t id) {
			const auto ptr = try_require<T>(id);

			if (ptr != nullptr) {
				return *ptr;
			}

			log_print_out("failed to find resource: %u", id);
			abort();
		}

		template<class T>
		const T& require(const oak::string &name) {
			return require<T>(std::hash<oak::string>{}(name));
		}

	private:
		struct Resource {};

		oak::vector<Block> resourceHandles_;
	};



}