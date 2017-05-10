#pragma once

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
	T& getComponent(ComponentStorage& storage, EntityId entity) {
		oak_assert(ComponentHandleStorage::inst().getHandle(util::type_id<detail::BaseComponent, T>::id) == storage.getHandle());
		return *static_cast<T*>(storage.getComponent(entity));
	}

}


