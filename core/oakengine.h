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

namespace oak {

	template<class T>
	void addQueue(EventManager& manager) {
	}

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
		log_print_out("tid: %lu, name: %s", T::typeInfo.id, T::typeInfo.name.c_str());
		T* comp = static_cast<T*>(scene.addComponent(entity, T::typeInfo.id));
		new (comp) T{ std::forward<TArgs>(args)... };
		return *static_cast<T*>(comp);
	}

	template<class T>
	bool hasComponent(EntityId entity, const Scene& scene) {
		return scene.hasComponent(entity, T::typeInfo.id);
	}

}
