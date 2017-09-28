#pragma once

#include <bitset>

#include "container.h"
#include "component.h"
#include "entity_id.h"

namespace oak {
	
	namespace config {
		constexpr size_t MAX_FLAGS = 32;
	}

	class ComponentStorage;

	class Scene {
	public:
		EntityId createEntity();
		void destroyEntity(EntityId entity);
		void activateEntity(EntityId entity);
		void deactivateEntity(EntityId entity);

		bool isEntityAlive(EntityId entity) const;
		bool isEntityActive(EntityId entity) const;

		void* addComponent(EntityId entity, size_t tid);
		void addComponent(EntityId entity, size_t tid, const void* ptr);
		void removeComponent(EntityId entity, size_t tid);

		void* getComponent(EntityId entity, size_t tid);
		const void* getComponent(EntityId entity, size_t tid) const;
		bool hasComponent(EntityId entity, size_t tid) const;
		const std::bitset<config::MAX_COMPONENTS>& getComponentFilter(EntityId entity) const;

		void init();
		void terminate();

		void update();
		void reset();

		void save(const oak::string& path);
		void load(const oak::string& path);

		void addComponentStorage(ComponentStorage *storage);
		ComponentStorage& getComponentStorage(size_t tid);
		const ComponentStorage& getComponentStorage(size_t tid) const;

		inline const oak::vector<EntityId>& getEntities() const { return entities_; }
		inline size_t getEntityCount() const { return entities_.size(); }
	private:
		oak::vector<EntityId> entities_;
		oak::vector<EntityId> killed_;

		oak::vector<std::bitset<config::MAX_COMPONENTS>> componentMasks_;
		oak::vector<std::bitset<config::MAX_FLAGS>> flags_;

		//stores the generation of each entity
		oak::vector<uint32_t> generations_;
		//stores the indices free for reuse
		oak::deque<uint32_t> freeIndices_;

		oak::vector<ComponentStorage*> componentPools_;
		oak::vector<ComponentStorage*> ownsPools_;
		
		void ensureSize(size_t size);
		void removeAllComponents(EntityId entity);
	};

}
