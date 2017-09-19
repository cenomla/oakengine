#include "component_storage.h"

#include "type_info.h"
#include "component.h"

namespace oak {

	ComponentStorage::ComponentStorage(const TypeInfo *tinfo) : 
		tinfo_{ tinfo }, 
		allocator_{ &oalloc_freelist, 8192, tinfo->size, 8 } {}

	ComponentStorage::~ComponentStorage() {
		size_t size = tinfo_->size;
		for (auto& component : components_) {
			if (component != nullptr) {
				allocator_.deallocate(component, size);
			}
		}
	}

	void* ComponentStorage::addComponent(EntityId entity) {
		auto component = makeValid(entity);
		tinfo_->construct(component);
		return component;
	}

	void ComponentStorage::addComponent(EntityId entity, const void *ptr) {
		auto component = makeValid(entity);
		tinfo_->copyConstruct(component, ptr);
	}

	void ComponentStorage::removeComponent(EntityId entity) {
		auto component = components_[entity];
		tinfo_->destruct(component);
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
			component = allocator_.allocate(tinfo_->size);
		}
		return component;
	}

}
