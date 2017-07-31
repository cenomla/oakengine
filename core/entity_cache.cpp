#include "entity_cache.h"

#include <algorithm>

#include "event_manager.h"
#include "event_queue.h"
#include "scene.h"
#include "scene_events.h"
#include "scene_utils.h"

namespace oak {

	void EntityCache::update(const Scene& scene) {
		bool needsSort = false;

		for (const auto& evt : EventManager::inst().getQueue<EntityActivateEvent>()) {
			ensureSize(evt.entity.index);
			//check if the entity matches the filter
			const auto& compFilter = scene.getComponentFilter(evt.entity);
			if (filter(scene, evt.entity, compFilter)) {
				if (!contains_[evt.entity]) {
					addEntity(evt.entity);
					needsSort = true;
				}
			} else {
				if (contains_[evt.entity]) {
					removeEntity(evt.entity);
					needsSort = true;
				}
			}
		}

		for (const auto& evt : EventManager::inst().getQueue<EntityDeactivateEvent>()) {
			ensureSize(evt.entity.index);
			//if the entity is contained then remove it
			if (contains_[evt.entity]) {
				removeEntity(evt.entity);
				needsSort = true;
			}
		}

		if (needsSort) {
			sort();
		}
	}

	bool EntityCache::contains(EntityId entity) {
		return contains_.size() > entity.index && contains_[entity];
	}

	void EntityCache::addEntity(EntityId entity) {
		entities_.push_back(entity);
		contains_[entity] = true;
	}

	void EntityCache::removeEntity(EntityId entity) {
		entities_.erase(std::remove(std::begin(entities_), std::end(entities_), entity), std::end(entities_));
		contains_[entity] = false;
	}

	void EntityCache::sort() {
		std::sort(std::begin(entities_), std::end(entities_), [](const EntityId& first, const EntityId& second) {
			return first.index < second.index;
		});
	}

	void EntityCache::ensureSize(size_t size) {
		if (contains_.size() <= size) {
			contains_.resize(size + 1);
		}
	}

	static const size_t ecid = util::type_id<detail::BaseComponent, EventComponent>::id;
	static const size_t pcid = util::type_id<detail::BaseComponent, PrefabComponent>::id;

	bool EntityCache::filter(const Scene& scene, EntityId entity, const std::bitset<config::MAX_COMPONENTS>& compFilter) {
		return (compFilter & componentFilter_) == componentFilter_ &&
			(compFilter[ecid] ? 
				(getComponent<const EventComponent>(entity, scene).filter & eventFilter_) == eventFilter_ :
				true) &&
			(compFilter[pcid] && prefabFilter_ != 0 ? 
				getComponent<const PrefabComponent>(entity, scene).id == prefabFilter_ :
				true);
	}
}