#pragma once

#include <vector>
#include <unordered_map>

#include "util/typeid.h"
#include "memory/memory_manager.h"
#include "system.h"
#include "log.h"

namespace oak {

	class ResourceListHandlerBase {
	public:
		virtual ~ResourceListHandlerBase() {}
	};

	template<typename T>
	class ResourceListHandler : public ResourceListHandlerBase {
	public:
		template<typename... TArgs>
		T* addResource(const std::string &name, TArgs&&... args) {
			auto it = resources_.emplace(name, T{ std::forward<TArgs>(args)... });
			return &it.first->second;
		}

		T* getResource(const std::string &name) {
			const auto it = resources_.find(name);
			if (it != std::end(resources_)) {
				return &it->second;
			} else {
				return nullptr;
			}
		}
	private:
		std::unordered_map<std::string, T> resources_;
	};

	class ResourceManager : public System {
	public:
		ResourceManager(Engine &engine) : System(engine, "resource_manager") {}

		void init() override {
			MemoryManager::inst().createPool(1, 512_kb);
		}

		void destroy() override {
			for (auto block : resourceHandles_) {
				if (block.ptr != nullptr) {
					static_cast<ResourceListHandlerBase*>(block.ptr)->~ResourceListHandlerBase();
					MemoryManager::inst().deallocate(block.ptr, block.size, 1);
				}
			}
		}

		template<typename T, typename... TArgs>
		T& add(const std::string &name, TArgs&&... args) {
			size_t tid = util::type_id<Resource, T>::id;

			if (tid >= resourceHandles_.size()) { resourceHandles_.resize(tid+1); }
			if (resourceHandles_.at(tid).ptr == nullptr) { 
				auto ptr = static_cast<ResourceListHandler<T>*>(MemoryManager::inst().allocate(sizeof(ResourceListHandler<T>), 1));
				new (ptr) ResourceListHandler<T>{};
				resourceHandles_[tid] = { ptr, sizeof(ResourceListHandler<T>) }; 
			}

			auto plh = static_cast<ResourceListHandler<T>*>(resourceHandles_.at(tid).ptr);
			return *plh->addResource(name, std::forward<TArgs>(args)...);
		}

		template<typename T>
		const T& require(const std::string &name) {
			size_t tid = util::type_id<Resource, T>::id;
			
			if (tid < resourceHandles_.size() && resourceHandles_.at(tid).ptr != nullptr) {
				auto plh = static_cast<ResourceListHandler<T>*>(resourceHandles_.at(tid).ptr);

				auto pres = plh->getResource(name);
				if (pres != nullptr) {
					return *pres;
				} else {
					log::cout << "failed to find resource: " << name << std::endl;
					std::exit(-1);
				}
			} else {
				log::cout << "failed to find resource: " << name << std::endl;
				std::exit(-1);
			}			
		}





	private:
		struct Resource {};

		std::vector<Block> resourceHandles_;
	};



}