#pragma once

#include "scene.h"
#include "entity.h"

namespace oak {

	class EntityCache {
	public:
		EntityCache(const Scene *scene);

		void update();

		const oak::vector<Entity>& entities() const { return entities_; }
		void clear() { entities_.clear(); };
		
		template<class T>
		void requireComponent() {
			filter_[util::type_id<detail::BaseComponent, T>::id] = true;
		}
	private:
		oak::vector<Entity> entities_;
		oak::vector<bool> contains_;
		std::bitset<config::MAX_COMPONENTS> filter_;
		const Scene *scene_;

		void addEntity(EntityId entity);
		void removeEntity(EntityId entity);
		void sort();
		void ensureSize(size_t size);
	};

}