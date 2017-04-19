#pragma once

#include "util/typeid.h"
#include "container.h"
#include "oak_alloc.h"
#include "engine.h"
#include "entity.h"


namespace oak {

	class Prefab {
	public:
		Prefab(EntityManager *manager);
		~Prefab();

		Entity createInstance(float depth) const;

		template <class T, typename... TArgs>
		T* addComponent(bool shared, TArgs&&... args) {
			size_t tid = util::type_id<Component, T>::id;
			void* comp = proxyAllocator.allocate(sizeof(T));
			new (comp) T{ std::forward<TArgs>(args)...};
			//ensure size
			if (tid >= storage_.size()) {
				storage_.resize(tid + 1);
			}
			storage_[tid] = { shared, comp };
			return static_cast<T*>(comp);
		}

		void* addComponent(bool shared, size_t tid) {
			const auto *thandle = Engine::inst().getSystem<ComponentHandleStorage>().getHandle(tid);
			void *comp = proxyAllocator.allocate(thandle->size());
			thandle->construct(comp);
			//ensure size
			if (tid >= storage_.size()) {
				storage_.resize(tid + 1);
			}
			storage_[tid] = { shared, comp };
			return comp;
		}

		void clear();
	private:
		oak::vector<std::pair<bool, void*>> storage_;
		EntityManager *manager_;
	};

}