#include "scene.h"

#include <algorithm>

#include "util/stream_puper.h"
#include "type_manager.h"
#include "oakengine.h"
#include "file_manager.h"
#include "scene_events.h"
#include "component_storage.h"

namespace oak {

	EntityId Scene::createEntity() {
		EntityId id;
		if (freeIndices_.empty()) {
			id = { static_cast<uint32_t>(generations_.size()), 0 };
			generations_.push_back(0);
		} else {
			uint32_t i = freeIndices_.front();
			id = { i, generations_[i] };
			freeIndices_.pop_front();
		}
		
		ensureSize(id.index);

		entities_.push_back(id);
		getEventQueue<EntityCreateEvent>().emit({ id });
		return id;
	}

	void Scene::destroyEntity(EntityId entity) {
		deactivateEntity(entity);
		generations_[entity]++;
		killed_.push_back(entity);
		getEventQueue<EntityDestroyEvent>().emit({ entity });
	}

	void Scene::activateEntity(EntityId entity) {
		flags_[entity][0] = true;
		getEventQueue<EntityActivateEvent>().emit({ entity });
	}

	void Scene::deactivateEntity(EntityId entity) {
		flags_[entity][0] = false;
		getEventQueue<EntityDeactivateEvent>().emit({ entity });
	}

	bool Scene::isEntityAlive(EntityId entity) const {
		return generations_.size() > entity.index ? generations_[entity] == entity.generation : false;
	}

	bool Scene::isEntityActive(EntityId entity) const {
		return flags_.size() > entity.index ? flags_[entity][0] : false;
	}

	void* Scene::addComponent(EntityId entity, size_t tid) {
		auto& mask = componentMasks_[entity];
		auto pool = componentPools_[tid];
		if (mask[tid]) {
			pool->removeComponent(entity);
		}
		mask[tid] = true;
		return pool->addComponent(entity);
	}

	void Scene::addComponent(EntityId entity, size_t tid, const void *ptr) {
		auto& mask = componentMasks_[entity];
		auto pool = componentPools_[tid];
		if (mask[tid]) {
			pool->removeComponent(entity);
		}
		mask[tid] = true;
		pool->addComponent(entity, ptr);
	}

	void Scene::removeComponent(EntityId entity, size_t tid) {
		componentMasks_[entity][tid] = false;
		componentPools_[tid]->removeComponent(entity);
	}

	void* Scene::getComponent(EntityId entity, size_t tid) {
		return componentPools_[tid]->getComponent(entity);
	}

	const void* Scene::getComponent(EntityId entity, size_t tid) const {
		return componentPools_[tid]->getComponent(entity);
	}

	bool Scene::hasComponent(EntityId entity, size_t tid) const {
		return componentMasks_[entity][tid];
	}

	const std::bitset<config::MAX_COMPONENTS>& Scene::getComponentFilter(EntityId entity) const {
		return componentMasks_[entity];
	}

	void Scene::init() {
		//create component storages for types in ComponentTypeManager
		for (auto it : ComponentTypeManager::inst().getTypes()) {
			auto ptr = static_cast<ComponentStorage*>(oak_allocator.allocate(sizeof(ComponentStorage)));
			new (ptr) ComponentStorage(it);
			ownsPools_.push_back(ptr);
			addComponentStorage(ptr);
		}
	}

	void Scene::terminate() {
		reset();
		for (auto it : ownsPools_) {
			it->~ComponentStorage();
			oak_allocator.deallocate(it, sizeof(ComponentStorage));
		}
		ownsPools_.clear();
	}

	void Scene::update() {
		for (const auto& e : killed_) {
			//remove all the entities components
			removeAllComponents(e);
			//remove the entity from the alive list and recycle the index
			entities_.erase(std::remove(std::begin(entities_), std::end(entities_), e), std::end(entities_));
			freeIndices_.push_back(e);
		}


		killed_.clear();
	}

	void Scene::reset() {
		auto& deactivateQueue = getEventQueue<EntityDeactivateEvent>();
		auto& destroyQueue = getEventQueue<EntityDestroyEvent>();
		for (const auto& e : entities_) { 
			deactivateQueue.emit({ e });			
			destroyQueue.emit({ e });
			removeAllComponents(e);
		}
		for (auto& a : flags_) {
			a.reset();
		}
		entities_.clear();
		generations_.clear();
		freeIndices_.clear();
	}

	void Scene::save(const oak::string& path) {
		auto file = FileManager::inst().openFile(path, true);
		
		auto info = ObjInfo::make<Scene>(nullptr, "scene");

		//header:
		//version
		//entity count
		//max component count
		//text:
		//for each entity:
		//	active
		//	component count
		//	for each component:
		//		component id
		//		component data

		StreamPuper puper{ &file };

		size_t version = 0;
		size_t entityCount = entities_.size();
		size_t maxComponents = config::MAX_COMPONENTS;

		pup(puper, version, ObjInfo::make<size_t>(&info, "version"));
		pup(puper, entityCount, ObjInfo::make<size_t>(&info, "entityCount"));
		pup(puper, maxComponents, ObjInfo::make<size_t>(&info, "maxComponents"));

		size_t i = 0;
		for (auto& entity : entities_) {
			auto& filter = getComponentFilter(entity);
			auto active = isEntityActive(entity);
			auto componentCount = filter.count();
			auto entityInfo = ObjInfo::make<EntityId>(&info, oak::string{ "entity[" } + std::to_string(i).c_str() + "]");
			pup(puper, active, ObjInfo::make<size_t>(&entityInfo, "active"));
			pup(puper, componentCount, ObjInfo::make<size_t>(&entityInfo, "componentCount"));
			for (size_t j = 0; j < config::MAX_COMPONENTS; j++) {
				if (filter[j]) {
					pup(puper, j, ObjInfo::make<size_t>(&entityInfo, "componentId"));
					auto ti = ComponentTypeManager::inst().getTypeInfo(j);
					ti->serialize(puper, getComponent(entity, j), entityInfo, ti->name);
				}
			}
			i++;
		}

		FileManager::inst().closeFile(file);
	}

	void Scene::load(const oak::string& path) {
		reset();

		auto file = FileManager::inst().openFile(path);
		
		auto info = ObjInfo::make<Scene>(nullptr, "scene");

		StreamPuper puper{ &file };
		puper.setIo(PuperIo::IN);

		size_t version, entityCount, maxComponents;

		pup(puper, version, ObjInfo::make<size_t>(&info, "version"));
		pup(puper, entityCount, ObjInfo::make<size_t>(&info, "entityCount"));
		pup(puper, maxComponents, ObjInfo::make<size_t>(&info, "maxComponents"));

		oak_assert(maxComponents == config::MAX_COMPONENTS);

		for (size_t i = 0; i < entityCount; i++) {
			auto entityInfo = ObjInfo::make<EntityId>(&info, oak::string{ "entity[" } + std::to_string(i).c_str() + "]");
			EntityId entity = createEntity();
			bool active;
			size_t componentCount;
			pup(puper, active, ObjInfo::make<bool>(&entityInfo, "active"));
			pup(puper, componentCount, ObjInfo::make<size_t>(&entityInfo, "componentCount"));
			for (size_t j = 0; j < componentCount; j++) {
				size_t componentId;
				pup(puper, componentId, ObjInfo::make<size_t>(&entityInfo, "componentId"));
				auto comp = addComponent(entity, componentId);
				auto ti = ComponentTypeManager::inst().getTypeInfo(componentId);
				ti->serialize(puper, comp, entityInfo, ti->name);
			}
			if (active) {
				activateEntity(entity);
			}
		}
		FileManager::inst().closeFile(file);
	}

	void Scene::addComponentStorage(ComponentStorage *storage) {
		size_t tid = storage->getTypeInfo()->id;
		if (componentPools_.size() <= tid) {
			componentPools_.resize(tid + 1);
		}
		componentPools_[tid] = storage;
	}

	ComponentStorage& Scene::getComponentStorage(size_t tid) {
		return *componentPools_[tid];
	}

	const ComponentStorage& Scene::getComponentStorage(size_t tid) const {
		return *componentPools_[tid];
	}

	void Scene::ensureSize(size_t size) {
		if (componentMasks_.size() <= size) {
			componentMasks_.resize(size + 1);
		}
		if (flags_.size() <= size) {
			flags_.resize(size + 1);
		}
	}

	void Scene::removeAllComponents(EntityId entity) {
		for (size_t i = 0; i < config::MAX_COMPONENTS; i++) {
			if (componentMasks_[entity][i]) {
				removeComponent(entity, i);
			}
		}
	}

}
