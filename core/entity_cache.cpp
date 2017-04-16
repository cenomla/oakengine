#include "entity_cache.h"

namespace oak {

	void EntityCache(const Scene *scene) : scene_{ scene } {}

	void EntityCache::update(const Scene &scene) {
		bool needsSort = false;

		for (const auto& e : activated) {
			ensureSize(e.index);
			//check if the entity matches the filter
			const auto& filter = scene_->getComponentFilter()
			if ((filter & filter_) == filter_) {
				if (!contains_[e]) {
					addEntity(e);
					needsSort = true;
				}
			} else {
				if (contains_[e]) {
					removeEntity(e);
					needsSort = true;
				}
			}
		}

		for (const auto& e : deactivated) {
			ensureSize(e.index);
			//if the entity is contained then remove it
			if (contains_[e]) {
				removeEntity(e);
				needsSort = true;
			}
		}

		if (needsSort) {
			sort();
		}
	}

	void EntityCache::addEntity(EntityId entity) {
		entities_.push_back(Entity{ entity, scene_ });
		contains_[entity] = true;
	}

	void EntityCache::removeEntity(EntityId entity) {
		entities_.erase(std::remove_if(std::begin(entities_), std::end(entities_), [&entity](const Entity& elem){ return elem.id() == entity; }), std::end(entities_));
		contains_[entity] = false;
	}

	void EntityCache::sort() {
		std::sort(std::begin(entities_), std::end(entities_), [](const Entity& first, const Entity& second) {
			return first.id() < second.id();
		});
	}

	void EntityCache::ensureSize(size_t size) {
		if (contains_.size() <= size) {
			contains_.resize(size + 1);
		}
	}
}