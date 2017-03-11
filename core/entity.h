#pragma once

#include <functional>
#include <array>
#include <bitset>

#include "config.h"
#include "memory/alloc.h"
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
		EntityManager(Engine &engine);
		
		void destroy() override;

		EntityManager(const EntityManager& other) = delete;
		void operator=(const EntityManager& other) = delete;

		Entity createEntity(uint8_t layer);
		void destroyEntity(const Entity &entity);
		void activateEntity(const Entity &entity);
		void deactivateEntity(const Entity &entity);
		bool isEntityActive(const Entity &entity) const;

		void* addComponent(uint32_t idx, uint32_t tid);
		void addComponent(uint32_t idx, uint32_t tid, void* ptr);
		void removeComponent(uint32_t idx, uint32_t tid);
		void deleteComponent(uint32_t idx, uint32_t tid);
		void removeAllComponents(uint32_t idx);

		void* getComponent(uint32_t idx, uint32_t tid);
		const void* getComponent(uint32_t idx, uint32_t tid) const;
		bool hasComponent(uint32_t idx, uint32_t tid) const;
		bool ownsComponent(uint32_t idx, uint32_t tid) const;

		void update();
		void reset();

		void disable();
		void enable();

		void addCache(EntityCache *system);

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
			std::bitset<config::MAX_COMPONENTS> componentMask;
			std::bitset<config::MAX_COMPONENTS> ownsMask;
			std::bitset<2> flags;
			oak::vector<bool> caches;
		};
		//stores the generation of each entity
		oak::vector<uint32_t> generation_;
		//stores the indices free for reuse
		oak::deque<uint32_t> freeIndices_;
		//stores all of the entities metadata
		oak::vector<EntityAttribute> entityAttributes_;
		//stores all the systems for entity caching
		oak::unordered_map<uint32_t, EntityCache*> caches_;
	};

	class Entity {
	public:
		Entity(uint64_t id, EntityManager *manager) 
			: id_{ id }, manager_{ manager } {}
		Entity(uint32_t index, uint32_t generation, uint8_t layer, EntityManager *manager) 
			: id_{ static_cast<uint64_t>(index) | (static_cast<uint64_t>(generation) << 32) | (static_cast<uint64_t>(layer) << 56) }, manager_{ manager } {}

		bool operator==(const Entity& other) const { return id_ == other.id_ && manager_ == other.manager_; }
		bool operator!=(const Entity& other) const { return !operator==(other); }

		inline uint32_t index() const { return static_cast<uint32_t>(id_ & ~(uint64_t{ 1 } << 32)); }
		inline uint32_t generation() const { return static_cast<uint32_t>((id_ >> 32) & ~(uint64_t{ 1 } << 24)); }
		inline uint32_t layer() const { return static_cast<uint32_t>((id_ >> 56) & ~(uint64_t{ 1 } << 8)); }
		inline uint64_t id() const { return id_; }

		inline void activate() { manager_->activateEntity(*this); }
		inline void deactivate() { manager_->deactivateEntity(*this); }
		inline void destroy() { manager_->destroyEntity(*this); }
		inline bool isActive() const { return manager_->isEntityActive(*this); }

		template<typename T, typename... TArgs>
		inline T& addComponent(TArgs&&... args) {
			T* comp = static_cast<T*>(manager_->addComponent(index(), util::type_id<Component, T>::id));
			new (comp) T{std::forward<TArgs>(args)...};
			return *static_cast<T*>(comp);
		}

		template<typename T>
		inline void removeComponent() {
			manager_->removeComponent(index(), util::type_id<Component, T>::id);
		}

		template<typename T>
		inline T& getComponent() {
			return *static_cast<T*>(manager_->getComponent(index(), util::type_id<Component, T>::id));
		}

		template<typename T>
		inline const T& getComponent() const {
			return *static_cast<const T*>(manager_->getComponent(index(), util::type_id<Component, T>::id));
		}

		template<typename T>
		inline bool hasComponent() const {
			return manager_->hasComponent(index(), util::type_id<Component, T>::id);
		}

		inline EntityManager* getManager() { return manager_; }
		inline const EntityManager* getManager() const { return manager_; }

		
	private:
		uint64_t id_;
		EntityManager *manager_;
	};

	class EntityCache {
	public:
		void setOnAdd(const std::function<void (const Entity &e)> &func);
		void setOnRemove(const std::function<void (const Entity &e)> &func);

		void addEntity(const Entity &e);
		void removeEntity(const Entity &e);

		inline oak::vector<Entity>& entities() { return entities_; }
		inline const oak::vector<Entity>& entities() const { return entities_; }

		void sort();
		inline const std::bitset<config::MAX_COMPONENTS>& getFilter() const { return filter_; }

		inline void clearEntities() { entities_.clear(); };
		
		template<typename T>
		inline void requireComponent() {
			filter_[util::type_id<Component, T>::id] = true;
		}
	private:

		oak::vector<Entity> entities_;
		std::function<void (const Entity &e)> onAdd_;
		std::function<void (const Entity &e)> onRemove_;

		std::bitset<config::MAX_COMPONENTS> filter_;
	};

}