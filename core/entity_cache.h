#pragma once

#include <bitset>

#include "util/type_id.h"
#include "container.h"
#include "entity_id.h"
#include "events.h"

namespace oak {

	class Scene;

	class EntityCache {
	public:

		void update(const Scene& scene);

		const oak::vector<EntityId>& entities() const { return entities_; }
		void clear() { entities_.clear(); };
		
		template<class T>
		void requireComponent() {
			componentFilter_[util::type_id<detail::BaseComponent, T>::id] = true;
		}

		template<class T>
		void requireEvent() {
			eventFilter_[util::type_id<detail::BaseEvent, T>::id] = true;
		}
	private:
		oak::vector<EntityId> entities_;
		oak::vector<bool> contains_;
		std::bitset<config::MAX_COMPONENTS> componentFilter_;
		std::bitset<config::MAX_EVENTS> eventFilter_;

		void addEntity(EntityId entity);
		void removeEntity(EntityId entity);
		void sort();
		void ensureSize(size_t size);
		bool filter(EntityId entity, const std::bitset<config::MAX_COMPONENTS>& compFilter, const Scene& scene);
	};

}