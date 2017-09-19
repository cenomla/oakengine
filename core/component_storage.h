#pragma once

#include "type_info.h"
#include "entity_id.h"
#include "container.h"

namespace oak {

	class TypeHandleBase;

	class ComponentStorage {
	public:
		ComponentStorage(const TypeInfo *tinfo);
		~ComponentStorage();

		void* addComponent(EntityId entity);
		void addComponent(EntityId entity, const void *ptr);
		void removeComponent(EntityId entity);

		void* getComponent(EntityId entity);
		const void* getComponent(EntityId entity) const;

		const TypeInfo* getTypeInfo() const { return tinfo_; }

	private:
		const TypeInfo *tinfo_;
		PoolAllocator allocator_;
		oak::vector<void*> components_;

		void* makeValid(EntityId entity);
	};

}
