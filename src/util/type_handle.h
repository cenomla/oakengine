#pragma once

namespace oak {

	struct TypeHandleBase {
		virtual void copy(void *src, void *target) const = 0;
		virtual void destruct(void *comp) const = 0;	
	};

	template <class T>
	struct TypeHandle : public TypeHandleBase {
		void copy(void *src, void *target) const override {
			*static_cast<T*>(target) = *static_cast<T*>(src);
		}
		void destruct(void *comp) const override {
			static_cast<T*>(comp)->~T();
		}
	};

}