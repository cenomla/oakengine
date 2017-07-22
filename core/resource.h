#pragma once

#include <type_traits>

#include "util/mpl.h"
#include "oak_alloc.h"
#include "pup.h"

namespace oak {

	namespace detail {

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

		template<class T>
		struct Resource {
			static void destroy(T& res) {
				if constexpr(has_destroy<T>::value) {
					res.destroy();
				}
			}
		};

	}

	template<class T>
	class ResourceStorage : public detail::ResourceStorageBase {
	public:
		typedef T value_type;

		ResourceStorage(oak::Allocator *allocator = &oalloc_freelist) : allocator_{ allocator } {};
		
		~ResourceStorage() {
			for (auto it : resources_) {
				detail::Resource<T>::destroy(*it.second);
				it.second->~T();
				allocator_->deallocate(it.second, sizeof(T));
			}
			resources_.clear();
		}

		template<class... TArgs>
		T& add(const oak::string& name, TArgs&&... args) {
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

		void remove(const oak::string& name) {
			size_t id = std::hash<oak::string>{}(name);

			auto it = resources_.find(id);
			if (it != std::end(resources_)) {
				detail::Resource<T>::destroy(*it.second);
				it.second->~T();
				allocator_->deallocate(it.second, sizeof(T));
				resources_.erase(it);
			}
		}

		T* require(size_t id) {
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

}