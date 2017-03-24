#include "entity.h"

#include <algorithm>

#include "engine.h"
#include "events.h"

namespace oak {

	EntityManager::EntityManager(Engine& engine) : System{ engine, "entity_manager" } {}

	void EntityManager::destroy() {
		reset();
	}

	Entity EntityManager::createEntity(float depth) {
		uint32_t idx;
		if (freeIndices_.size() > 1024) {
			idx = freeIndices_.front();
			freeIndices_.pop_front();
		} else {
			idx = generation_.size();
			generation_.push_back(0);
		}

		entities_.alive.emplace_back(idx, 0, this);
		entityAttributes_.resize(idx + 1);
		entityAttributes_[idx].depth = depth;
		return entities_.alive.back();
	}

	void EntityManager::destroyEntity(const Entity& entity) {
		entities_.deactivated.push_back(entity);
		entities_.killed.push_back(entity);
	}

	void EntityManager::activateEntity(const Entity& entity) {
		Engine::inst().getEventManager().emitEvent(EntityActivateEvent{ entity });
		entities_.activated.push_back(entity);
	}

	void EntityManager::deactivateEntity(const Entity& entity) {
		Engine::inst().getEventManager().emitEvent(EntityDeactivateEvent{ entity });
		entities_.deactivated.push_back(entity);
	}

	bool EntityManager::isEntityActive(const Entity& entity) const {
		return entityAttributes_[entity.index()].flags[0];
	}

	float EntityManager::getEntityDepth(const Entity& entity) const {
		return entityAttributes_[entity.index()].depth;
	}

	void EntityManager::setEntityDepth(const Entity& entity, float depth) {
		entityAttributes_[entity.index()].depth = depth;
	}

	void* EntityManager::addComponent(uint32_t idx, uint32_t tid) {
		auto& attribute = entityAttributes_[idx];
		
		if (attribute.ownsMask[tid]) {
			removeComponent(idx, tid);
		} else {
			attribute.components[tid] = componentPools_[tid].proxy.allocate(0);
			attribute.ownsMask[tid] = true;
		}

		attribute.componentMask[tid] = true;
		return attribute.components[tid];
	}

	void EntityManager::addComponent(uint32_t idx, uint32_t tid, void *ptr) {
		auto& attribute = entityAttributes_[idx];
		if (attribute.ownsMask[tid]) {
			removeComponent(idx, tid);
			deleteComponent(idx, tid);
		}
		attribute.componentMask[tid] = true;
		attribute.components[tid] = ptr;
	}

	void EntityManager::removeComponent(uint32_t idx, uint32_t tid) {
		auto& attribute = entityAttributes_[idx];
		if (attribute.componentMask[tid] && attribute.ownsMask[tid]) {
			Engine::inst().getSystem<ComponentHandleStorage>().getHandle(tid)->destruct(attribute.components[tid]);
		}
		attribute.componentMask[tid] = false;
	}

	void EntityManager::deleteComponent(uint32_t idx, uint32_t tid) {
		auto& attribute = entityAttributes_[idx];
		if (attribute.components[tid] != nullptr && attribute.ownsMask[tid]) {
			componentPools_[tid].proxy.deallocate(attribute.components[tid], 0);
			attribute.components[tid] = nullptr;
			attribute.ownsMask[tid] = false;
		}
	}

	void EntityManager::removeAllComponents(uint32_t idx) {
		for (size_t i = 0; i < config::MAX_COMPONENTS; i++) {
			removeComponent(idx, i);
			deleteComponent(idx, i);
		}
	}

	void* EntityManager::getComponent(uint32_t idx, uint32_t tid) {
		return entityAttributes_[idx].components[tid];
	}

	const void* EntityManager::getComponent(uint32_t idx, uint32_t tid) const {
		return entityAttributes_[idx].components[tid];
	}

	bool EntityManager::hasComponent(uint32_t idx, uint32_t tid) const {
		return entityAttributes_[idx].componentMask[tid];
	}

	bool EntityManager::ownsComponent(uint32_t idx, uint32_t tid) const {
		return entityAttributes_[idx].ownsMask[tid];
	}

	void EntityManager::update() {
		oak::unordered_map<size_t, EntityCache*> cachesToSort;
		//remove all entities to remove
		for (auto& e : entities_.activated) {
			auto& attribute = entityAttributes_[e.index()];
			attribute.flags[0] = true;

			//add entity to relavent caches
			for (auto& s : caches_) {
				size_t sysType = s.first;

				EntityCache& sys = *s.second;
				//if the entity has all the components that the system requires then add it to the system
				if ((sys.getFilter() & attribute.componentMask) == sys.getFilter()) {
					if (sysType >= attribute.caches.size() || !attribute.caches[sysType]) {
						sys.addEntity(e);
						cachesToSort.insert(s);
						if (sysType >= attribute.caches.size()) {
							attribute.caches.resize(sysType + 1);
						}
						attribute.caches[sysType] = true;
					}
				}
				//if the entity doesn't match the component requirement but still is in the system remove it
				else if (attribute.caches.size() > sysType && attribute.caches[sysType]) {
					sys.removeEntity(e);
					attribute.caches[sysType] = false;
				}
			}

		}

		for (auto& sys : cachesToSort) {
			sys.second->sort();
		}

		for (auto& e : entities_.deactivated) {
			auto& attribute = entityAttributes_[e.index()];
			attribute.flags[0] = false;

			for (auto& s : caches_) {
				size_t sysType = s.first;

				EntityCache& sys = *s.second;
				//if the entity is apart of a system then remove it
				if (attribute.caches.size() > sysType && attribute.caches[sysType]) {
					sys.removeEntity(e);
					attribute.caches[sysType] = false;
				}
			}
		}

		for (auto& e : entities_.killed) {
			size_t idx = e.index();
			//remove all the entities components
			removeAllComponents(idx);

			//remove the entity from the alive list and recycle the index
			entities_.alive.erase(std::remove(std::begin(entities_.alive), std::end(entities_.alive), e), std::end(entities_.alive));

			++generation_[idx];
			freeIndices_.push_back(idx);
		}


		entities_.clear();
	}

	void EntityManager::reset() {
		for (auto& e : entities_.alive) {
			removeAllComponents(e.index());
		}
		for (auto& s : caches_) {
			s.second->clearEntities();
		}
		for (auto& a : entityAttributes_) {
			a.flags.reset();
			a.caches.clear();
		}
		entities_.reset();
		generation_.clear();
		freeIndices_.clear();
	}

	void EntityManager::enable() {
		for (auto& entity : entities_.alive) {
			auto& attribute = entityAttributes_[entity.index()];
			//if the entity is supposed to be active then add it to the appropriate caches
			if (attribute.flags[0]) {
				for (auto& system : caches_) {
					if (attribute.caches[system.first]) {
						system.second->addEntity(entity);
					}
				}
			}
		}
	}

	void EntityManager::disable() {
		for (auto& entity : entities_.alive) {
			auto& attribute = entityAttributes_[entity.index()];
			for (auto& system : caches_) {
				//if the entity is apart of the system remove it but do not set its inactive flag
				if (attribute.caches[system.first]) {
					system.second->removeEntity(entity);
				}
			}
		}
	}

	void EntityManager::addCache(EntityCache *cache) {
		caches_.insert({caches_.size(), cache});
	}

	void EntityCache::setOnAdd(const std::function<void (const Entity& e)>& func) {
		onAdd_ = func;
	}

	void EntityCache::setOnRemove(const std::function<void (const Entity& e)>& func) {
		onRemove_ = func;
	}

	void EntityCache::addEntity(const Entity& e) {
		entities_.push_back(e);
		if (onAdd_) {
			onAdd_(e);
		}
	}

	void EntityCache::removeEntity(const Entity& e) {
		if (onRemove_) {
			onRemove_(e);
		}
		entities_.erase(std::remove(std::begin(entities_), std::end(entities_), e), std::end(entities_));
	}

	void EntityCache::sort() {
		std::sort(std::begin(entities_), std::end(entities_), [](const Entity& first, const Entity& second) -> bool {
			return first.index() < second.index();
		});
	}

}