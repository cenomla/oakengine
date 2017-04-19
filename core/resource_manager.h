#pragma once

#include "util/typeid.h"
#include "container.h"
#include "log.h"

namespace oak {

	namespace detail {
		struct BaseResource {};
	}

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

	class ResourceManager {
	private:
		static ResourceManager *instance;
	public:
		static ResourceManager& inst() { return *instance; }

		ResourceManager();
		~ResourceManager();

		template<class T, class... TArgs>
		T& add(const oak::string &name, TArgs&&... args) {
			size_t tid = util::type_id<detail::BaseResource, T>::id;

			if (resourceHandles_.size() <= tid) { 
				resourceHandles_.resize(tid+1);
			}
			if (resourceHandles_[tid] == nullptr) { 
				auto ptr = static_cast<ResourceListHandler<T>*>(oak_allocator.allocate(sizeof(ResourceListHandler<T>)));
				new (ptr) ResourceListHandler<T>{};
				resourceHandles_[tid] = ptr;
			}

			auto ptr = static_cast<ResourceListHandler<T>*>(resourceHandles_[tid]);
			return *ptr->addResource(std::hash<oak::string>{}(name), std::forward<TArgs>(args)...);
		}

		template<class T>
		const T* try_require(size_t id) {
			size_t tid = util::type_id<detail::BaseResource, T>::id;
			
			if (tid < resourceHandles_.size() && resourceHandles_[tid] != nullptr) {
				const auto handle = static_cast<ResourceListHandler<T>*>(resourceHandles_[tid]);

				const auto res = handle->getResource(id);
				if (res != nullptr) {
					return res;
				}
			}

			return nullptr;
		}

		template<class T>
		const T& require(size_t id) {
			const auto res = try_require<T>(id);

			if (res != nullptr) {
				return *res;
			}

			log_print_out("failed to find resource: %u", id);
			abort();
		}

		template<class T>
		const T& require(const oak::string &name) {
			return require<T>(std::hash<oak::string>{}(name));
		}

	private:
		oak::vector<ResourceListHandlerBase*> resourceHandles_;
	};

}
