#pragma once

#include "oak_assert.h"
#include "oak_alloc.h"
#include "container.h"
#include "type_info.h"

namespace oak {

	namespace detail {
		struct BaseResource {};
	}

	template<class T>
	TypeInfo makeResourceInfo(const oak::string& name) {
		return makeTypeInfo<detail::BaseResource, T>(name);
	}

	class ResourceStorage {
	public:
		ResourceStorage(const TypeInfo *tinfo, oak::Allocator *allocator = &oalloc_freelist, size_t pageSize = 10_mb);

		~ResourceStorage();

		void* setDefault();
		void* add(const oak::string& name);
		void remove(const oak::string& name);
		const void* require(const oak::string& name);
		const void* require(size_t id);
	private:
		oak::unordered_map<size_t, void*> resources_;
		void *defaultResource_;
		PoolAllocator allocator_;
		const TypeInfo *typeInfo_;
	};

	class ResourceManager {
	private:
		static ResourceManager *instance;
	public:
		inline static ResourceManager& inst() { 
			oak_assert(instance != nullptr);
			return *instance; 
		}

		ResourceManager();
		~ResourceManager();

		ResourceStorage& get(const TypeInfo *tinfo);
	private:
		oak::vector<ResourceStorage*> resourceHandles_;
	};

}
