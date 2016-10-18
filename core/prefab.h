#pragma once

#include <unordered_map>

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
		void addComponent(TArgs&&... args) {
			size_t tid = util::type_id<Component, T>::id;
			Block comp = { MemoryManager::inst().allocate(sizeof(T)), sizeof(T) };
			new (comp.ptr) T{ std::forward<TArgs>(args)...};
			storage_.insert({ tid, comp });
		}

		void clear();
	private:
		std::unordered_map<size_t, Block> storage_;
		EntityManager *manager_;
	};

}