#pragma once

#include <bitset>

#include "util/type_id.h"
#include "container.h"
#include "component.h"
#include "entity_id.h"
#include "events.h"

namespace oak {

	class Scene;

	class EntityCache {
	public:

		void update(const Scene& scene);
		bool contains(const Scene& scene, EntityId entity);

		inline const oak::vector<EntityId>& entities() const { return entities_; }
		inline void clear() { entities_.clear(); };
		
		template<class T>
		inline void requireComponent() {
			componentFilter_[util::type_id<detail::BaseComponent, T>::id] = true;
		}

		template<class T>
		inline void requireEvent() {
			eventFilter_[util::type_id<detail::BaseEvent, T>::id] = true;
		}

		inline void requirePrefab(size_t id) {
			prefabFilter_ = id;
		}


	private:
		oak::vector<EntityId> entities_;
		oak::vector<bool> contains_;
		std::bitset<config::MAX_COMPONENTS> componentFilter_;
		std::bitset<config::MAX_EVENTS> eventFilter_;
		size_t prefabFilter_ = 0;

		void addEntity(EntityId entity);
		void removeEntity(EntityId entity);
		void sort();
		void ensureSize(size_t size);
		bool filter(const Scene& scene, EntityId entity, const std::bitset<config::MAX_COMPONENTS>& compFilter);
	};

}