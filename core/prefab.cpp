#include "prefab.h"

#include "oakengine.h"
#include "type_manager.h"
#include "core_components.h"

namespace oak {

	Prefab::Prefab(const oak::string& name, Scene& scene) : name_{ name }, scene_{ &scene } {}
	
	Prefab::~Prefab() {
		clear();
	}

	EntityId Prefab::createInstance() const {
		EntityId entity = scene_->createEntity();

		oak::addComponent<PrefabComponent>(entity, *scene_, std::hash<oak::string>{}(name_));
		
		for (size_t i = 0; i < storage_.size(); i++) {
			const auto& it = storage_[i];
			if (it != nullptr) {
				scene_->addComponent(entity, i, it);
			}
		}

		return entity;
	}

	void* Prefab::addComponent(size_t tid) {
		auto ti = ComponentTypeManager::inst().getTypeInfo(tid);
		void *comp = oak_allocator.allocate(ti->size);
		ti->construct(comp);
		//ensure size
		if (tid >= storage_.size()) {
			storage_.resize(tid + 1);
		}
		storage_[tid] = comp;
		return comp;
	}

	void Prefab::clear() {
		for (size_t i = 0; i < storage_.size(); i++) {
			const auto& it = storage_[i];
			if (it != nullptr) {
				auto ti = ComponentTypeManager::inst().getTypeInfo(i);
				ti->destruct(it);
				oak_allocator.deallocate(it, ti->size);
			}
		}
		storage_.clear();
	}

}
