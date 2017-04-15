#pragma once

#include <functional>
#include <array>
#include <bitset>

#include "config.h"
#include "memory/oak_alloc.h"
#include "container.h"
#include "util/type_handle_storage.h"
#include "system.h"

namespace oak {

	constexpr uint32_t MIN_FREE_INDICES = 1024;

	struct Component {};
	using ComponentHandleStorage = TypeHandleStorage<Component, config::MAX_COMPONENTS>;

	class Entity;
	class EntityCache;

	class EntityManager : public System {
	public:
		EntityManager(Engine& engine);
		
		void destroy() override;

		EntityManager(const EntityManager& other) = delete;
		void operator=(const EntityManager& other) = delete;

		Entity createEntity(uint64_t idx);
		Entity convertEntity(const Entity& entity);

		Entity createEntity(float depth);

		void destroyEntity(const Entity& entity);
		void activateEntity(const Entity& entity);
		void deactivateEntity(const Entity& entity);
		bool isEntityActive(const Entity& entity) const;

		float getEntityDepth(const Entity& entity) const;
		void setEntityDepth(const Entity& entity, float depth);

		void* addComponent(uint64_t idx, size_t tid);
		void addComponent(uint64_t idx, size_t tid, void* ptr);
		void removeComponent(uint64_t idx, size_t tid);
		void deleteComponent(uint64_t idx, size_t tid);
		void removeAllComponents(uint64_t idx);

		void* getComponent(uint64_t idx, size_t tid);
		const void* getComponent(uint64_t idx, size_t tid) const;
		bool hasComponent(uint64_t idx, size_t tid) const;
		bool ownsComponent(uint64_t idx, size_t tid) const;

		void update();
		void reset();

		void addCache(EntityCache *system);

		template<class T>
		void initComponentPool() {
			size_t tid = util::type_id<Component, T>::id;
			auto& pool = componentPools_[tid];
			pool.allocator = { MemoryManager::inst().allocate(4000000), 4000000, sizeof(T) };
			pool.proxy = oak_allocator<void>{ &pool.allocator, 4000000 };
		}

		inline size_t getEntityCount() const { return entities_.alive.size(); }
	private:
		struct {
			oak::vector<Entity> alive;
			oak::vector<Entity> activated;
			oak::vector<Entity> deactivated;
			oak::vector<Entity> killed;

			inline void clear() {
				activated.clear();
				deactivated.clear();
				killed.clear();	
			}

			inline void reset() {
				clear();
				alive.clear();
			}

		} entities_;

		struct EntityAttribute {
			std::array<void*, config::MAX_COMPONENTS> components{ nullptr };
			oak::vector<bool> caches;
			std::bitset<config::MAX_COMPONENTS> componentMask;
			std::bitset<config::MAX_COMPONENTS> ownsMask;
			std::bitset<2> flags; //active, alive
			uint32_t layer = 0;
			float depth = 0.0f;
		};
		//stores the generation of each entity
		oak::vector<uint64_t> generation_;
		//stores the indices free for reuse
		oak::deque<uint64_t> freeIndices_;
		//stores all of the entities metadata
		oak::vector<EntityAttribute> entityAttributes_;
		//stores all the systems for entity caching
		oak::unordered_map<uint32_t, EntityCache*> caches_;
		//converting of previous indices to current onces
		oak::vector<uint64_t> idxConverter_;

		struct ComponentPool {
			oak_allocator<void> proxy{ nullptr };
			PoolAllocator allocator;
		};
		std::array<ComponentPool, config::MAX_COMPONENTS> componentPools_;
	};

	class Entity {
	public:
		Entity() = default;
		Entity(uint64_t index, EntityManager *manager)
			: index_{ index }, manager_{ manager } {}

		bool operator==(const Entity& other) const { return index_ == other.index_ && manager_ == other.manager_; }
		bool operator!=(const Entity& other) const { return !operator==(other); }

		inline uint64_t index() const { return index_; }
		
		inline float get_depth() const { return manager_->getEntityDepth(*this); }
		inline void set_depth(float depth) { manager_->setEntityDepth(*this, depth); }

		inline void activate() { manager_->activateEntity(*this); }
		inline void deactivate() { manager_->deactivateEntity(*this); }
		inline void destroy() { manager_->destroyEntity(*this); }
		inline bool isActive() const { return manager_->isEntityActive(*this); }

		template<typename T, typename... TArgs>
		inline T& addComponent(TArgs&&... args) {
			T* comp = static_cast<T*>(manager_->addComponent(index_, util::type_id<Component, T>::id));
			new (comp) T{std::forward<TArgs>(args)...};
			return *static_cast<T*>(comp);
		}

		template<typename T>
		inline void removeComponent() {
			manager_->removeComponent(index_, util::type_id<Component, T>::id);
		}

		template<typename T>
		inline T& getComponent() {
			return *static_cast<T*>(manager_->getComponent(index_, util::type_id<Component, T>::id));
		}

		template<typename T>
		inline const T& getComponent() const {
			return *static_cast<const T*>(manager_->getComponent(index_, util::type_id<Component, T>::id));
		}

		template<typename T>
		inline bool hasComponent() const {
			return manager_->hasComponent(index_, util::type_id<Component, T>::id);
		}

		inline EntityManager* getManager() { return manager_; }
		inline const EntityManager* getManager() const { return manager_; }

		
	private:
		uint64_t index_;
		EntityManager *manager_;
	};

	class EntityCache {
	public:
		void setOnAdd(const std::function<void (const Entity& e)>& func);
		void setOnRemove(const std::function<void (const Entity& e)>& func);

		void addEntity(const Entity& e);
		void removeEntity(const Entity& e);

		inline oak::vector<Entity>& entities() { return entities_; }
		inline const oak::vector<Entity>& entities() const { return entities_; }

		void sort();
		inline const std::bitset<config::MAX_COMPONENTS>& getFilter() const { return filter_; }

		inline void clear() { entities_.clear(); };
		
		template<typename T>
		inline void requireComponent() {
			filter_[util::type_id<Component, T>::id] = true;
		}
	private:

		oak::vector<Entity> entities_;
		std::function<void (const Entity& e)> onAdd_;
		std::function<void (const Entity& e)> onRemove_;

		std::bitset<config::MAX_COMPONENTS> filter_;
	};

}