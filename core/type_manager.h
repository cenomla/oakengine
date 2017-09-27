#pragma once

#include "oak_assert.h"

#include "util/type_id.h"
#include "type_info.h"
#include "container.h"

namespace oak {

	template<class U>
	class TypeManager {
	private:
		static TypeManager<U>* instance;
	public:
		static TypeManager<U>& inst() {
			oak_assert(instance != nullptr);
			return *instance;
		}

		TypeManager() {
			oak_assert(instance == nullptr);
			instance = this;
		}

		~TypeManager() {
			oak_assert(instance != nullptr);
			instance = nullptr;
		}
		
		template<class T>
		void addType() {
			size_t tid = T::typeInfo.id;
			if (types_.size() <= tid) {
				types_.resize(tid + 1);
			}
			if (types_[tid] != nullptr) { return; }
			types_[tid] = &T::typeInfo;
			nameMap_[T::typeInfo.name] = tid;
		}


		inline const TypeInfo* getTypeInfo(const oak::string& name) const {
			const auto& it = nameMap_.find(name);
			if (it != std::end(nameMap_)) {
				return getTypeInfo(it->second);
			}
			return nullptr;
		}

		inline const TypeInfo* getTypeInfo(size_t tid) const {
			return types_[tid];
		}

		inline const oak::vector<const TypeInfo*>& getTypes() const {
			return types_;
		}

	private:
		oak::vector<const TypeInfo*> types_;
		oak::unordered_map<oak::string, size_t> nameMap_;
	};


	template<class U>
	TypeManager<U>* TypeManager<U>::instance = nullptr;


}
