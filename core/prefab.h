#pragma once

#include "util/type_id.h"
#include "container.h"
#include "scene.h"

namespace oak {

	class Prefab {
	public:
		Prefab(const oak::string& name, Scene& scene);
		~Prefab();

		EntityId createInstance() const;
		void* addComponent(size_t tid);

		template <class T, class... TArgs>
		T& addComponent(TArgs&&... args) {
			size_t tid = T::typeInfo.id;
			void* comp = addComponent(tid);
			new (comp) T{ std::forward<TArgs>(args)...};
			return *static_cast<T*>(comp);
		}

		void clear();
	private:
		oak::vector<void*> storage_;
		oak::string name_;
		Scene *scene_;
	};

}
