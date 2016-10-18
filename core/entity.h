#pragma once

#include <functional>
#include <vector>
#include <deque>
#include <array>
#include <unordered_map>
#include <bitset>

#include "memory/memory_manager.h"
#include "util/type_handle.h"
#include "util/typeid.h"
#include "system.h"

namespace oak {

	constexpr size_t MAX_COMPONENTS = 64;
	constexpr uint32_t MIN_FREE_INDICES = 1024;

	struct Component {};

	class Entity;
	class EntityCache;

	class EntityManager : public System {
	public:
		EntityManager(Engine &engine);

		void init() override;
		void destroy() override;

		EntityManager(const EntityManager& other) = delete;
		void operator=(const EntityManager& other) = delete;

		template<typename T>
		void addComponentHandle() {
			size_t tid = util::type_id<Component, T>::id;
			if (componentHandles_[tid].ptr != nullptr) { return; }
			Block block = { MemoryManager::inst().allocate(sizeof(TypeHandle<T>)), sizeof(TypeHandle<T>) };
			new (block.ptr) TypeHandle<T>{};
			componentHandles_[tid] = block;
		}

		template<typename T>
		const TypeHandle<T>& getComponentHandle() {
			size_t tid = util::type_id<Component, T>::id;
			return *static_cast<TypeHandle<T>*>(getComponentHandle(tid));
		}

		inline TypeHandleBase* getComponentHandle(size_t tid) {
			return static_cast<TypeHandleBase*>(componentHandles_[tid].ptr);
		}

		Entity createEntity(uint8_t layer);
		void destroyEntity(const Entity &entity);
		void activateEntity(const Entity &entity);
		void deactivateEntity(const Entity &entity);
		bool isEntityActive(const Entity &entity) const;

		void* addComponent(uint32_t idx, uint32_t tid, size_t size);
		void removeComponent(uint32_t idx, uint32_t tid);
		void deleteComponent(uint32_t idx, uint32_t tid);
		void removeAllComponents(uint32_t idx);

		void* getComponent(uint32_t idx, uint32_t tid);
		const void* getComponent(uint32_t idx, uint32_t tid) const;
		bool hasComponent(uint32_t idx, uint32_t tid) const;

		void update();
		void reset();

		void disable();
		void enable();

		void addCache(EntityCache *system);

		inline size_t getEntityCount() const { return entities_.alive.size(); }
	private:
		struct {
			std::vector<Entity> alive;
			std::vector<Entity> activated;
			std::vector<Entity> deactivated;
			std::vector<Entity> killed;

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
			std::vector<bool> caches;
			std::bitset<MAX_COMPONENTS> componentMask;
			std::array<Block, MAX_COMPONENTS> components;
			std::bitset<2> flags;
		};
		//stores the generation of each entity
		std::vector<uint32_t> generation_;
		//stores the indices free for reuse
		std::deque<uint32_t> freeIndices_;
		//stores all of the entities metadata
		std::vector<EntityAttribute> entityAttributes_;
		//stores all the systems for entity caching
		std::unordered_map<uint32_t, EntityCache*> caches_;
		//component handles
		std::array<Block, MAX_COMPONENTS> componentHandles_;
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
		inline uint8_t layer() const { return static_cast<uint8_t>((id_ >> 56) & ~(uint64_t{ 1 } << 8)); }
		inline uint64_t id() const { return id_; }

		inline void activate() { manager_->activateEntity(*this); }
		inline void deactivate() { manager_->deactivateEntity(*this); }
		inline void destroy() { manager_->destroyEntity(*this); }
		inline bool isActive() const { return manager_->isEntityActive(*this); }

		template<typename T, typename... TArgs>
		inline T& addComponent(TArgs&&... args) {
			T* comp = static_cast<T*>(manager_->addComponent(index(), util::type_id<Component, T>::id, sizeof(T)));
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

		inline const std::vector<Entity>& entities() { return entities_; }

		void sort();
		inline const std::bitset<MAX_COMPONENTS>& getFilter() const { return filter_; }

		inline void clearEntities() { entities_.clear(); };
		
		template<typename T>
		inline void requireComponent() {
			filter_[util::type_id<Component, T>::id] = true;
		}
	private:

		std::vector<Entity> entities_;
		std::function<void (const Entity &e)> onAdd_;
		std::function<void (const Entity &e)> onRemove_;

		std::bitset<MAX_COMPONENTS> filter_;
	};

}