#pragma once

#include "entity_id.h"

namespace oak {

	using ComponentHandleStorage = TypeHandleStorage<detail::BaseComponent, config::MAX_COMPONENTS>;

	class Scene {
	public:

		void init();		
		void terminate();

		EntityId createEntity(uint64_t idx);

		void destroyEntity(EntityId entity);
		void activateEntity(EntityId entity);
		void deactivateEntity(EntityId entity);

		bool isEntityAlive(EntityId entity) const;
		bool isEntityActive(EntityId entity) const;

		void addComponent(EntityId entity, size_t tid);
		void addComponent(EntityId entity, size_t tid, const void* ptr);
		void removeComponent(EntityId entity, size_t tid);


		void* getComponent(EntityId entity, size_t tid);
		const void* getComponent(EntityId entity, size_t tid) const;
		bool hasComponent(EntityId entity, size_t tid) const;
		const std::bitset<config::MAX_COMPONENTS>& getComponentFilter(EntityId entity);

		void update();

		template<class T>
		void initComponentPool() {
			size_t tid = util::type_id<detail::BaseComponent, T>::id;
			auto& pool = componentPools_[tid];
			pool.allocator = { MemoryManager::inst().allocate(4000000), 4000000, sizeof(T) };
			pool.proxy = OakAllocator<void>{ &pool.allocator, 4000000 };
		}

		size_t getEntityCount() const { return entities_.alive.size(); }
	private:
		oak::vector<EntityId> entities_;
		oak::vector<EntityId> killed_;

		std::vector<std::array<void*, config::MAX_COMPONENTS>> components_;
		std::vector<std::bitset<config::MAX_COMPONENTS>> componentMasks_;
		std::vector<std::bitset<4>> flags_;

		//stores the generation of each entity
		oak::vector<uint32_t> generations_;
		//stores the indices free for reuse
		oak::deque<uint32_t> freeIndices_;

		struct ComponentPool {
			OakAllocator<void> proxy{ nullptr };
			PoolAllocator allocator;
		};
		std::array<ComponentPool, config::MAX_COMPONENTS> componentPools_;
		
		
		void ensureSize(size_t size);
		void removeAllComponents(EntityId entity);
		void reset();
	};

}