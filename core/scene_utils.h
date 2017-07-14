#pragma once

#include <type_traits>

#include "oak_assert.h"

#include "util/type_id.h"
#include "util/mpl.h"
#include "component.h"
#include "component_storage.h"
#include "scene.h"

namespace oak {

	template<class T>
	ComponentStorage& getComponentStorage(Scene& scene) {
		return scene.getComponentStorage(util::type_id<detail::BaseComponent, typename std::remove_const<T>::type>::id);
	}

	template<class T>
	const ComponentStorage& getComponentStorage(const Scene& scene) {
		return scene.getComponentStorage(util::type_id<detail::BaseComponent, typename std::remove_const<T>::type>::id);
	}

	template<class T>
	T& getComponent(mpl::const_type<T, ComponentStorage>& storage, EntityId entity) {
		oak_assert(ComponentTypeManager::inst().getHandle(util::type_id<detail::BaseComponent, typename std::remove_const<T>::type>::id) == storage.getHandle());
		return *static_cast<T*>(storage.getComponent(entity));
	}

	template<class T>
	T& getComponent(mpl::const_type<T, Scene>& scene, EntityId entity) {
		return *static_cast<T*>(scene.getComponent(entity, util::type_id<detail::BaseComponent, typename std::remove_const<T>::type>::id));
	}

	template<class T, class... TArgs>
	T& addComponent(Scene& scene, EntityId entity, TArgs&&... args) {
		T* comp = static_cast<T*>(scene.addComponent(entity, util::type_id<detail::BaseComponent, T>::id));
		new (comp) T{ std::forward<TArgs>(args)... };
		return *static_cast<T*>(comp);
	}

	template<class T>
	bool hasComponent(const Scene& scene, EntityId entity) {
		return scene.hasComponent(entity, util::type_id<detail::BaseComponent, T>::id);
	}

}


