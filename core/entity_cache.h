#pragma once

#include <bitset>

#include "util/type_id.h"
#include "container.h"
#include "entity_id.h"

namespace oak {

	class Scene;

	class EntityCache {
	public:

		void update(const Scene& scene);

		const oak::vector<EntityId>& entities() const { return entities_; }
		void clear() { entities_.clear(); };
		
		template<class T>
		void requireComponent() {
			filter_[util::type_id<detail::BaseComponent, T>::id] = true;
		}
	private:
		oak::vector<EntityId> entities_;
		oak::vector<bool> contains_;
		std::bitset<config::MAX_COMPONENTS> filter_;

		void addEntity(EntityId entity);
		void removeEntity(EntityId entity);
		void sort();
		void ensureSize(size_t size);
	};

}