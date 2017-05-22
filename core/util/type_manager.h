#pragma once

#include "oak_assert.h"

#include "type_id.h"
#include "type_handle.h"

#include "container.h"

namespace oak {

	template<class U>
	class TypeManager {
	private:
		static TypeManager<U>* instance;
	public:
		static constexpr size_t HSize = sizeof(TypeHandle<U, U>);

		static TypeManager<U>& inst() {
			oak_assert(instance != nullptr);
			return *instance;
		}

		TypeManager() {
			oak_assert(instance == nullptr);
			instance = this;
		}

		~TypeManager() {
			instance = nullptr;
			for (auto& ptr : handles_) {
				if (ptr != nullptr) {
					ptr->~TypeHandleBase();
					oak_allocator.deallocate(ptr, HSize);
				}
			}
		}

		template<class T>
		void addHandle(const oak::string &name) {
			auto ptr = static_cast<TypeHandle<U, T>*>(oak_allocator.allocate(HSize));
			new (ptr) TypeHandle<U, T>{ name };
			size_t tid = ptr->getId();
			if (handles_.size() <= tid) {
				handles_.resize(tid + 1);
			}
			if (handles_[tid] != nullptr) { return; }
			handles_[tid] = ptr;
			nameMap_[name] = tid;
		}

		template<class T>
		const TypeHandle<U, T>& getHandle() const {
			size_t tid = util::type_id<U, T>::id;
			return *static_cast<const TypeHandle<U, T>*>(handles_[tid]);
		}

		const TypeHandleBase* getHandle(const oak::string& name) const {
			return handles_[getId(name)];
		}

		const TypeHandleBase* getHandle(size_t tid) const {
			return handles_[tid];
		}

		size_t getId(const oak::string &name) const {
			const auto& it = nameMap_.find(name);
			if (it != std::end(nameMap_)) {
				return it->second;
			}
			return 0;
		}


	private:
		oak::vector<TypeHandleBase*> handles_;
		oak::unordered_map<oak::string, size_t> nameMap_;
	};


	template<class U>
	TypeManager<U>* TypeManager<U>::instance = nullptr;

}
