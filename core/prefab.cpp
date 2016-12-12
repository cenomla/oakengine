#include "prefab.h"

namespace oak {

	Prefab::Prefab(EntityManager *manager) : manager_{ manager } {}
	
	Prefab::~Prefab() {
		clear();
	}

	Entity Prefab::createInstance(uint8_t layer) const {
		Entity entity = manager_->createEntity(layer);
		for (size_t i = 0; i < storage_.size(); i++) {
			const auto& it = storage_[i];
			if (it.second.ptr != nullptr) {
				if (it.first) {
					manager_->addComponent(entity.index(), i, it.second.ptr);
				} else {
					void *comp = manager_->addComponent(entity.index(), i);
					auto ch = manager_->getComponentHandle(i);
					ch->copy(it.second.ptr, comp);
				}
			}
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