#include "scene.h"

#include "event_manager.h"
#include "scene_events.h"
#include "component_storage.h"

namespace oak {

	void Scene::terminate() {
		for (const auto& e : entities_) {
			removeAllComponents(e);
		}
		for (auto& a : flags_) {
			a.reset();
		}
		entities_.clear();
		generation_.clear();
		freeIndices_.clear();
		componentPools_.clear();
	}

	EntityId Scene::createEntity() {
		EntityId id;
		if (freeIndices_.empty()) {
			id = { static_cast<uint32_t>(generations_.size()), 0) };
			generation_.push_back(0);
		} else {
			uint32_t i = freeIndices_.front();
			id = { i, generation_[i] };
			freeIndices_.pop_front();
		}
		
		ensureSize(id.index);

		entities_.push_back(id);
		return id;
	}

	void Scene::destroyEntity(EntityId entity) {
		generations_[entity]++;
		killed_.push_back(entity);
	}

	void Scene::activateEntity(EntityId entity) {
		flags_[entity][0] = true;
		EventManager::inst().getQueue<EntityActivateEvent>().emit({ entity });
	}

	void Scene::deactivateEntity(EntityId entity) {
		flags_[entity][0] = false;
		EventManager::inst().getQueue<EntityDeactivateEvent>().emit({ entity });
	}

	bool Scene::isEntityAlive(EntityId entity) const {
		return generations_[entity] == entity.generation;
	}

	bool Scene::isEntityActive(EntityId entity) const {
		return flags_[entity][0];
	}

	void Scene::addComponent(EntityId entity, size_t tid) {
		componentMasks_[entity][tid] = true;
		componentPools_[tid]->addComponent(entity);
	}

	void Scene::addComponent(EntityId entity, size_t tid, const void *ptr) {
		componentMasks_[entity][tid] = true;
		componentPools_[tid]->addComponent(entity, ptr);
	}

	void Scene::removeComponent(EntityId entity, size_t tid) {
		componentMask_[entity][tid] = false;
		componentPools_[tid]->removeComponent(entity);
	}

	void* Scene::getComponent(EntityId entity, size_t tid) {
		return componentPools_[tid]->getComponent(entity);
	}

	const void* Scene::getComponent(EntityId entity, size_t tid) const {
		return componentPools_[tid]->getComponent(entity);
	}

	bool Scene::hasComponent(EntityId entity, size_t tid) const {
		return componentMasks_[entity][tid];
	}

	void Scene::update() {
		for (const auto& e : killed_) {
			//remove all the entities components
			removeAllComponents(e);
			//remove the entity from the alive list and recycle the index
			entities_.alive.erase(std::remove(std::begin(entities_.alive), std::end(entities_.alive), e), std::end(entities_.alive));
			freeIndices_.push_back(e);
		}


		killed_.clear();
	}

	void Scene::addComponentStorage(size_t tid, ComponentStorage& storage) {
		if (componentPools_.size() <= tid) {
			componentPools_.resize(tid + 1);
		}
		componentPools_[tid] = &storage;
	}

	void Scene::ensureSize(size_t size) {
		if (componentMasks_.size() <= size) {
			componentMasks_.resize(size + 1);
		}
		if (flags_.size() <= size) {
			flags_.resize(size + 1);
		}
	}

	void Scene::removeAllComponents(EntityId entity) {
		for (size_t i = 0; i < config::MAX_COMPONENTS; i++) {
			if (componentMasks_[entity][i]) {
				removeComponent(entity, i);
			}
		}
	}

}
