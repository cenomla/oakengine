#include "resource_manager.h"

#include "type_info.h"

namespace oak {

	ResourceStorage::ResourceStorage(const TypeInfo *tinfo, oak::Allocator *allocator, size_t pageSize) : defaultResource_{ nullptr }, allocator_{ allocator, pageSize, tinfo->size }, typeInfo_{ tinfo } {};

	ResourceStorage::~ResourceStorage() {
		for (auto it : resources_) {
			typeInfo_->destroy(it.second);
			typeInfo_->destruct(it.second);
			allocator_.deallocate(it.second, typeInfo_->size);
		}
		resources_.clear();
	}

	void* ResourceStorage::setDefault() {
		if (defaultResource_ == nullptr) {
			defaultResource_ = allocator_.allocate(typeInfo_->size);
		} else {
			typeInfo_->destroy(defaultResource_);
			typeInfo_->destruct(defaultResource_);
		}
		return defaultResource_;
	}

	void* ResourceStorage::add(const oak::string& name) {
		auto id = std::hash<oak::string>{}(name);

		auto it = resources_.find(id);
		if (it == std::end(resources_)) {
			auto ptr = allocator_.allocate(typeInfo_->size);
			resources_.insert({ id, ptr });
			return ptr;
		} else {
			auto ptr = it->second;
			typeInfo_->destroy(ptr);
			typeInfo_->destruct(ptr);
			return ptr;
		}
	}

	void ResourceStorage::remove(const oak::string& name) {
		auto id = std::hash<oak::string>{}(name);

		auto it = resources_.find(id);
		if (it != std::end(resources_)) {
			auto ptr = it->second;
			typeInfo_->destroy(ptr);
			typeInfo_->destruct(ptr);
			allocator_.deallocate(ptr, typeInfo_->size);
			resources_.erase(it);
		}
	}

	const void* ResourceStorage::require(const oak::string& name) {
		auto id = std::hash<oak::string>{}(name);

		auto it = resources_.find(id);
		if (it != std::end(resources_)) {
			return it->second;
		} else if (defaultResource_ != nullptr) {
			return defaultResource_;
		}
		return nullptr;
	}

	ResourceManager *ResourceManager::instance = nullptr;

	ResourceManager::ResourceManager() {
		oak_assert(instance == nullptr);
		instance = this;
	}

	ResourceManager::~ResourceManager() {
		instance = nullptr;

		for (auto it : resourceHandles_) {
			if (it != nullptr) {
				it->~ResourceStorage();
				oak_allocator.deallocate(it, sizeof(ResourceStorage));
			}
		}
		resourceHandles_.clear();
	}

	ResourceStorage& ResourceManager::get(const TypeInfo *tinfo) {
		auto tid = tinfo->id;
		if (!(tid < resourceHandles_.size() && resourceHandles_[tid])) {
			if (resourceHandles_.size() <= tid) { 
				resourceHandles_.resize(tid + 1);
			}
			auto ptr = static_cast<ResourceStorage*>(oak_allocator.allocate(sizeof(ResourceStorage)));
			new (ptr) ResourceStorage{ tinfo };
			resourceHandles_[tid] = ptr;
			return *ptr;
		}

		return *resourceHandles_[tid];
	}

}
