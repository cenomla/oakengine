#pragma once

#include <type_traits>

#include "oak_assert.h"

#include "util/mpl.h"
#include "util/type_id.h"
#include "event_manager.h"
#include "input_manager.h"
#include "resource_manager.h"
#include "type_info.h"
#include "scene.h"
#include "component.h"
#include "component_storage.h"
#include "log.h"

namespace oak {

	//event api
	template<class T>
	void addEventQueue() {
		EventManager::inst().addQueue(&T::typeInfo);
	}

	template<class T>
	EventQueueIter<T>& getEventQueue() {
		return static_cast<EventQueueIter<T>&>(EventManager::inst().getQueue(&T::typeInfo));
	}

	//resource api
	template<class T, class... TArgs>
	T& setDefaultResource(TArgs&&... args) {
		auto& storage = ResourceManager::inst().get(&T::typeInfo);
		auto ptr = storage.setDefault();
		new (ptr) T{ std::forward<TArgs>(args)... };
		return *ptr;
	}
	
	template<class T, class... TArgs>
	T* addResource(size_t id, TArgs&&... args) {
		auto& storage = ResourceManager::inst().get(&T::typeInfo);
		auto ptr = storage.add(id);
		new (ptr) T{ std::forward<TArgs>(args)... };
		return static_cast<T*>(ptr);
	}

	template<class T, class... TArgs>
	T& addResource(const oak::string& name, TArgs&&... args) {
		return *addResource<T>(std::hash<oak::string>{}(name), std::forward<TArgs>(args)...);
	}

	template<class T>
	bool hasResource(size_t id) {
		auto& storage = ResourceManager::inst().get(&T::typeInfo);
		return storage.has(id);
	}
	
	template<class T>
	bool hasResource(const oak::string& name) {
		auto id = std::hash<oak::string>{}(name);
		return hasResource<T>(id);
	}

	template<class T>
	T* requireResource(size_t id) {
		return static_cast<T*>(ResourceManager::inst().get(&T::typeInfo).require(id));
	}

	template<class T>
	T& requireResource(const oak::string& name) {
		auto id = std::hash<oak::string>{}(name);
		auto ptr = requireResource<T>(id);
		if (ptr) {
			return *ptr;
		}
		log_print_err("failed to find resource: %s", name.c_str());
		abort();
	}



	//entity component api
	template<class T>
	mpl::const_type<T, ComponentStorage>& getComponentStorage(mpl::const_type<T, Scene>& scene) {
		return scene.getComponentStorage(T::typeInfo.id);
	}

	template<class T>
	T& getComponent(EntityId entity, mpl::const_type<T, ComponentStorage>& storage) {
		oak_assert(&T::typeInfo == storage.getTypeInfo());
		return *static_cast<T*>(storage.getComponent(entity));
	}

	template<class T>
	T& getComponent(EntityId entity, mpl::const_type<T, Scene>& scene) {
		return *static_cast<T*>(scene.getComponent(entity, T::typeInfo.id));
	}

	template<class... T, class... TArgs>
	std::tuple<T&...> getComponents(EntityId entity, TArgs&&... storage) {
		return { getComponent<T>(entity, storage)... };
	}

	template<class... T, class S>
	std::tuple<T&...> getComponents(EntityId entity, S&& scene) {
		return { getComponent<T>(entity, scene)... };
	}

	template<class T, class... TArgs>
	T& addComponent(EntityId entity, Scene& scene, TArgs&&... args) {
		T* comp = static_cast<T*>(scene.addComponent(entity, T::typeInfo.id));
		new (comp) T{ std::forward<TArgs>(args)... };
		return *static_cast<T*>(comp);
	}

	template<class T>
	bool hasComponent(EntityId entity, const Scene& scene) {
		return scene.hasComponent(entity, T::typeInfo.id);
	}

}

