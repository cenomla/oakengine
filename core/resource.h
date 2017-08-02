#pragma once

#include <atomic>

#include "resource_manager.h"
#include "pup.h"

namespace oak {

	template<class T>
	struct Resource;

	template<class T>
	void pup(Puper& puper, Resource<T>& resource, const ObjInfo& info) {
		auto id = resource.id;
		puper.pup(resource.id, info);
		if (id != resource.id) {
			resource.ptr = nullptr;
		}
	}

	template<class T>
	struct Resource {
		template<class TF> 
		friend void pup(Puper& puper, Resource<TF>& resource, const ObjInfo& info);

		Resource() : id{ 0 }, ptr{ nullptr } {}
		Resource(size_t i, const T* p = nullptr) : id{ i }, ptr{ p } {}
		Resource(const oak::string& name) : Resource(std::hash<oak::string>{}(name)) {}

		Resource(const Resource& other) : id{ other.id }, ptr{ other.ptr.load() } {}
		inline void operator=(const Resource& other) { id = other.id; ptr = other.ptr.load(); }

		inline const T operator*() const {
			get();
			return *ptr;
		}

		inline const T* operator->() const {
			get();
			return ptr;
		}

		inline operator const T*() const {
			get();
			return ptr;
		}

		inline void get() const {
			if (!ptr) {
				ptr = ResourceManager::inst().require<T>(id);
			}
			oak_assert(ptr);
		}

		size_t id = 0;
		mutable std::atomic<const T*> ptr = nullptr;
	};

}