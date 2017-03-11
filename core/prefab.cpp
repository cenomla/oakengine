#include "prefab.h"

namespace oak {

	Prefab::Prefab(EntityManager *manager) : manager_{ manager } {}
	
	Prefab::~Prefab() {
		clear();
	}

	Entity Prefab::createInstance(uint8_t layer) const {
		Entity entity = manager_->createEntity(layer);
		const auto& chs = Engine::inst().getSystem<ComponentHandleStorage>();
		for (size_t i = 0; i < storage_.size(); i++) {
			const auto& it = storage_[i];
			if (it.second != nullptr) {
				if (it.first) {
					manager_->addComponent(entity.index(), i, it.second);
				} else {
					void *comp = manager_->addComponent(entity.index(), i);
					const auto& ch = chs.getHandle(i);
					ch->construct(it.second, comp);
				}
			}
		}

		return entity;
	}

	void Prefab::clear() {
		const auto& chs = Engine::inst().getSystem<ComponentHandleStorage>();
		for (size_t i = 0; i < storage_.size(); i++) {
			const auto &it = storage_[i];
			if (it.second != nullptr) {
				const auto& ch = chs.getHandle(i);
				ch->destruct(it.second);
				proxyAllocator.deallocate(it.second, ch->size());
			}
		}
		storage_.clear();
	}

}