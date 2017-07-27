#include "prefab.h"

#include "core_components.h"
#include "scene_utils.h"

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
		const auto& handle = ComponentTypeManager::inst().getHandle(tid);
		void *comp = oak_allocator.allocate(handle->getSize());
		handle->construct(comp);
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
				const auto& handle = ComponentTypeManager::inst().getHandle(i);
				handle->destruct(it);
				oak_allocator.deallocate(it, handle->getSize());
			}
		}
		storage_.clear();
	}

}