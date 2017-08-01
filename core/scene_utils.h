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
	mpl::const_type<T, ComponentStorage>& getComponentStorage(mpl::const_type<T, Scene>& scene) {
		return scene.getComponentStorage(util::type_id<detail::BaseComponent, typename std::remove_const<T>::type>::id);
	}

	template<class T>
	T& getComponent(EntityId entity, mpl::const_type<T, ComponentStorage>& storage) {
		oak_assert(ComponentTypeManager::inst().getHandle(util::type_id<detail::BaseComponent, typename std::remove_const<T>::type>::id) == storage.getHandle());
		return *static_cast<T*>(storage.getComponent(entity));
	}

	template<class T>
	T& getComponent(EntityId entity, mpl::const_type<T, Scene>& scene) {
		return *static_cast<T*>(scene.getComponent(entity, util::type_id<detail::BaseComponent, typename std::remove_const<T>::type>::id));
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
		T* comp = static_cast<T*>(scene.addComponent(entity, util::type_id<detail::BaseComponent, T>::id));
		new (comp) T{ std::forward<TArgs>(args)... };
		return *static_cast<T*>(comp);
	}

	template<class T>
	bool hasComponent(EntityId entity, const Scene& scene) {
		return scene.hasComponent(entity, util::type_id<detail::BaseComponent, T>::id);
	}

	void save(const oak::string& path, Scene& scene);
	void load(const oak::string& path, Scene& scene);

}


