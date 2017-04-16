#pragma once

#include "util/typeid.h"
#include "entity_id.h"

namespace oak {

	class Entity {
	public:

		bool operator==(const Entity& other) const { return id_ == other.id_ && scene_ == other.scene_; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

		const EntityId& id() const { return id_; }
		
		void activate() { scene_->activateEntity(id_); }
		void deactivate() { scene_->deactivateEntity(id_); }
		void destroy() { scene_->destroyEntity(id_); }

		bool isAlive() const { return scene_->isEntityAlive(id_); }
		bool isActive() const { return scene_->isEntityActive(id_); }

		template<class T, class... TArgs>
		T& addComponent(TArgs&&... args) {
			T* comp = static_cast<T*>(scene_->addComponent(id_, util::type_id<detail::BaseComponent, T>::id));
			new (comp) T{std::forward<TArgs>(args)...};
			return *static_cast<T*>(comp);
		}

		template<class T>
		void removeComponent() {
			scene_->removeComponent(id_, util::type_id<detail::BaseComponent, T>::id);
		}

		template<class T>
		T& getComponent() {
			return *static_cast<T*>(scene_->getComponent(id_, util::type_id<detail::BaseComponent, T>::id));
		}

		template<class T>
		const T& getComponent() const {
			return *static_cast<const T*>(scene_->getComponent(id_, util::type_id<detail::BaseComponent, T>::id));
		}

		template<class T>
		bool hasComponent() const {
			return scene_->hasComponent(id_, util::type_id<detail::BaseComponent, T>::id);
		}

		Scene* getManager() { return scene_; }
		const Scene* getManager() const { return scene_; }
		
	private:
		EntityId id_;
		Scene *scene_;
	};

	

}