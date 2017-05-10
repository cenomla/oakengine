#pragma once

#include "entity_id.h"
#include "container.h"

namespace oak {

	class TypeHandleBase;

	class ComponentStorage {
	public:
		ComponentStorage(const TypeHandleBase *handle);
		~ComponentStorage();

		void* addComponent(EntityId entity);
		void addComponent(EntityId entity, const void *ptr);
		void removeComponent(EntityId entity);

		void* getComponent(EntityId entity);
		const void* getComponent(EntityId entity) const;

		const TypeHandleBase* getHandle() const { return handle_; }

	private:
		PoolAllocator allocator_;
		oak::vector<void*> components_;
		const TypeHandleBase *handle_;

		void* makeValid(EntityId entity);
	};

}