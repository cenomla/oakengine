#pragma once

#include <array>
#include <unordered_map>

#include "typeid.h"
#include "type_handle.h"
#include "memory/memory_manager.h"
#include "system.h"

namespace oak {

	template<class U, size_t C>
	class TypeHandleStorage : public System {
	public:

		TypeHandleStorage(Engine &engine) : System{ engine, "type_handle_storage" } {}

		~TypeHandleStorage() {
			for (auto &block : handles_) {
				if (block.ptr != nullptr) {
					static_cast<TypeHandleBase*>(block.ptr)->~TypeHandleBase();
					MemoryManager::inst().deallocate(block.ptr, block.size);
					block.ptr = nullptr;
				}
			}
		}

		template<class T>
		inline void addHandle(const std::string &name) {
			size_t tid = util::type_id<U, T>::id;
			if (handles_[tid].ptr != nullptr) { return; }
			size_t hsize = sizeof(TypeHandle<T>);
			TBlock<TypeHandleBase> block = { static_cast<TypeHandleBase*>(MemoryManager::inst().allocate(hsize)), hsize };
			new (block.ptr) TypeHandle<T>{ name };
			handles_[tid] = block;
			nameMap_[name] = tid;
		}

		template<typename T>
		inline const TypeHandle<T>& getHandle() const {
			size_t tid = util::type_id<U, T>::id;
			return *static_cast<const TypeHandle<T>*>(getHandle(tid));
		}

		inline const TypeHandleBase* getHandle(size_t tid) const {
			return handles_[tid].ptr;
		}

		inline size_t getId(const std::string &name) const {
			const auto& it = nameMap_.find(name);
			if (it != std::end(nameMap_)) {
				return it->second;
			}
			return 0;
		}


	private:
		std::array<TBlock<TypeHandleBase>, C> handles_;
		std::unordered_map<std::string, size_t> nameMap_;
	};


}