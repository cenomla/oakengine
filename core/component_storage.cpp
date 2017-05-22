#include "component_storage.h"

#include "util/type_handle.h"
#include "component.h"

namespace oak {

	ComponentStorage::ComponentStorage(const oak::string &compName) : 
	handle_{ ComponentTypeManager::inst().getHandle(compName) },
	allocator_{ &oalloc_freelist, 8192, handle_->getSize(), 8 }
	{}

	ComponentStorage::~ComponentStorage() {
		size_t size = handle_->getSize();
		for (auto& component : components_) {
			if (component != nullptr) {
				allocator_.deallocate(component, size);
			}
		}
	}

	void* ComponentStorage::addComponent(EntityId entity) {
		auto component = makeValid(entity);
		handle_->construct(component);
		return component;
	}

	void ComponentStorage::addComponent(EntityId entity, const void *ptr) {
		auto component = makeValid(entity);
		handle_->construct(component, ptr);
	}

	void ComponentStorage::removeComponent(EntityId entity) {
		auto component = components_[entity];
		handle_->destruct(component);
	}

	void* ComponentStorage::getComponent(EntityId entity) {
		return components_[entity];
	}

	const void* ComponentStorage::getComponent(EntityId entity) const {
		return components_[entity];
	}

	void* ComponentStorage::makeValid(EntityId entity) {
		if (components_.size() <= entity.index) {
			components_.resize(entity.index + 1);
		}
		auto& component = components_[entity];
		if (component == nullptr) {
			component = allocator_.allocate(handle_->getSize());
		}
		return component;
	}

}