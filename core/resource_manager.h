#pragma once

#include <type_traits>

#include "util/type_id.h"
#include "oak_assert.h"
#include "container.h"
#include "log.h"

namespace oak {

	namespace detail {

		struct BaseResource {};

		class ResourceStorageBase {
		public:
			virtual ~ResourceStorageBase() {};
		};

		template<typename T>
		struct has_destroy {
			template<typename U, void (U::*)()> struct SFINAE {};
			template<typename U> static char Test(SFINAE<U, &U::destroy>*);
			template<typename U> static int Test(...);
			static constexpr bool value = sizeof(Test<T>(0)) == sizeof(char);
		};
	}

	template<class T>
	class ResourceStorage : public detail::ResourceStorageBase {
		static void destroy(T& res) {
			if constexpr(detail::has_destroy<T>::value) {
				res.destroy();
			}
		}
	public:
		ResourceStorage(oak::Allocator *allocator = &oalloc_freelist) : allocator_{ allocator } {};
		
		~ResourceStorage() {
			for (auto it : resources_) {
				destroy(*it.second);
				it.second->~T();
				allocator_->deallocate(it.second, sizeof(T));
			}
			resources_.clear();
		}

		template<class... TArgs>
		inline T& add(const oak::string& name, TArgs&&... args) {
			size_t id = std::hash<oak::string>{}(name);

			auto it = resources_.find(id);
			if (it == std::end(resources_)) {
				auto ptr = static_cast<T*>(allocator_->allocate(sizeof(T)));
				new (ptr) T{ std::forward<TArgs>(args)... };
				resources_.insert({ id, ptr });
				return *ptr;
			}

			return *it->second;
		}

		inline void remove(const oak::string& name) {
			size_t id = std::hash<oak::string>{}(name);

			auto it = resources_.find(id);
			if (it != std::end(resources_)) {
				destroy(*it.second);
				it.second->~T();
				allocator_->deallocate(it.second, sizeof(T));
				resources_.erase(it);
			}
		}

		inline T* require(size_t id) {
			auto it = resources_.find(id);

			if (it != std::end(resources_)) {
				return it->second;
			}

			return nullptr;
		}

	private:
		oak::unordered_map<size_t, T*> resources_;
		oak::Allocator *allocator_;
	};

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
		inline ResourceStorage<T>& get() {
			size_t tid = util::type_id<detail::BaseResource, T>::id;

			if (!(tid < resourceHandles_.size() && resourceHandles_[tid])) {
				if (resourceHandles_.size() <= tid) { 
					resourceHandles_.resize(tid+1);
				}
				auto ptr = static_cast<ResourceStorage<T>*>(oak_allocator.allocate(sizeof(ResourceStorage<T>)));
				new (ptr) ResourceStorage<T>{};
				resourceHandles_[tid] = ptr;
				return *ptr;
			}

			return *static_cast<ResourceStorage<T>*>(resourceHandles_[tid]);
		}

		template<class T>
		inline T* require(size_t id) {
			return get<T>().require(id);
		}

		template<class T>
		inline T& require(const oak::string &name) {
			size_t id = std::hash<oak::string>{}(name);
			auto r = require<T>(id);
			if (r) {
				return *r;
			}
			log_print_err("failed to find resource: %s", name.c_str());
			abort();
		}

	private:
		oak::vector<detail::ResourceStorageBase*> resourceHandles_;
	};

}
