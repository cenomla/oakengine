#pragma once

#include <type_traits>

#include "oak_assert.h"

#include "util/type_id.h"
#include "component_storage.h"
#include "scene.h"

namespace oak {

	template<class T>
	ComponentStorage& getComponentStorage(Scene& scene) {
		return scene.getComponentStorage(util::type_id<detail::BaseComponent, T>::id);
	}

	template<class T>
	const ComponentStorage& getComponentStorage(const Scene& scene) {
		return scene.getComponentStorage(util::type_id<detail::BaseComponent, T>::id);
	}

	template<class T, class U>
	T& getComponent(U&& storage, EntityId entity) {
		oak_assert(ComponentHandleStorage::inst().getHandle(util::type_id<detail::BaseComponent, typename std::remove_const<T>::type>::id) == storage.getHandle());
		return *static_cast<T*>(storage.getComponent(entity));
	}

	template<class T, class... TArgs>
	T& addComponent(Scene& scene, EntityId entity, TArgs&&... args) {
		T* comp = static_cast<T*>(scene.addComponent(entity, util::type_id<detail::BaseComponent, T>::id));
		new (comp) T{ std::forward<TArgs>(args)... };
		return *static_cast<T*>(comp);
	}

}


