#pragma once

#include "oak_assert.h"

#include "type_id.h"
#include "type_handle.h"

#include "container.h"

namespace oak {

	template<class U>
	class TypeHandleStorage {
	private:
		static TypeHandleStorage<U>* instance;
	public:
		static constexpr size_t HSize = sizeof(TypeHandle<U>);

		static TypeHandleStorage<U>& inst() {
			oak_assert(instance != nullptr);
			return *instance;
		}

		TypeHandleStorage() {
			oak_assert(instance == nullptr);
			instance = this;
		}

		~TypeHandleStorage() {
			instance = nullptr;
			for (auto& ptr : handles_) {
				if (ptr != nullptr) {
					static_cast<TypeHandleBase*>(ptr)->~TypeHandleBase();
					oak_allocator.deallocate(ptr, HSize);
				}
			}
		}

		template<class T>
		void addHandle(const oak::string &name) {
			size_t tid = util::type_id<U, T>::id;
			if (handles_.size() <= tid) {
				handles_.resize(tid + 1);
			}
			if (handles_[tid] != nullptr) { return; }
			void *ptr = oak_allocator.allocate(HSize);
			new (ptr) TypeHandle<T>{ name };
			handles_[tid] = ptr;
			nameMap_[name] = tid;
		}

		template<class T>
		const TypeHandle<T>& getHandle() const {
			size_t tid = util::type_id<U, T>::id;
			return *static_cast<const TypeHandle<T>*>(handles_[tid]);
		}

		const TypeHandleBase* getHandle(size_t tid) const {
			return static_cast<TypeHandleBase*>(handles_[tid]);
		}

		size_t getId(const oak::string &name) const {
			const auto& it = nameMap_.find(name);
			if (it != std::end(nameMap_)) {
				return it->second;
			}
			return 0;
		}


	private:
		oak::vector<void*> handles_;
		oak::unordered_map<oak::string, size_t> nameMap_;
	};


	template<class U>
	TypeHandleStorage<U>* TypeHandleStorage<U>::instance = nullptr;

}
