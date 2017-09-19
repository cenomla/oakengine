#pragma once

#include "type_handle.h"
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
		const TypeHandleBase *handle_;
		PoolAllocator allocator_;
		oak::vector<void*> components_;

		void* makeValid(EntityId entity);
	};

}
