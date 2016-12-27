#pragma once

#include <string>
#include <vector>

#include "memory/memory_manager.h"
#include "util/typeid.h"
#include "entity.h"


namespace oak {

	class Prefab {
	public:
		Prefab(EntityManager *manager);
		~Prefab();

		Entity createInstance(uint8_t layer) const;

		template <class T, typename... TArgs>
		T* addComponent(bool shared, TArgs&&... args) {
			size_t tid = util::type_id<Component, T>::id;
			void* comp = MemoryManager::inst().allocate(sizeof(T));
			new (comp) T{ std::forward<TArgs>(args)...};
			//ensure size
			if (tid >= storage_.size()) {
				storage_.resize(tid + 1);
			}
			storage_[tid] = { shared, comp };
			return static_cast<T*>(comp);
		}

		void clear();
	private:
		std::vector<std::pair<bool, void*>> storage_;
		EntityManager *manager_;
	};

}