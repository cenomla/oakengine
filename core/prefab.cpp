#include "prefab.h"

namespace oak {

	Prefab::Prefab(EntityManager *manager) : manager_{ manager } {}
	
	Prefab::~Prefab() {
		clear();
	}

	Entity Prefab::createInstance(uint8_t layer) const {
		Entity entity = manager_->createEntity(layer);
		for (auto &it : storage_) {
			void *comp = manager_->addComponent(entity.index(), it.first, it.second.size);
			auto ch = manager_->getComponentHandle(it.first);
			ch->copy(it.second.ptr, comp);
		}

		return entity;
	}

	void Prefab::clear() {
		for (auto it : storage_) {
			manager_->getComponentHandle(it.first)->destruct(it.second.ptr);
			MemoryManager::inst().deallocate(it.second.ptr, it.second.size);
		}
		storage_.clear();
	}

}