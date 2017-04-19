#pragma once

#include "container.h"
#include "pup.h"

namespace oak {

	struct TypeHandleBase {
		TypeHandleBase(const oak::string &name, size_t size) : name_{ name }, size_{ size } {};
		virtual ~TypeHandleBase() {};
		virtual void construct(void *object) const = 0;
		virtual void construct(void *src, void *target) const = 0;
		virtual void copy(void *src, void *target) const = 0;
		virtual void destruct(void *comp) const = 0;
		virtual void serialize(Puper &puper, void *data, const ObjInfo &info) const = 0;
		size_t getSize() const { return size_; }
		const oak::string& getName() const { return name_; }
	protected:
		oak::string name_;
		size_t size_;
	};

	template <class T>
	struct TypeHandle : public TypeHandleBase {
		TypeHandle(const oak::string &name) : TypeHandleBase{ name, sizeof(T) } {};

		void construct(void *object) const override {
			new (object) T{};
		}

		void construct(void *object, void *src) const override {
			new (object) T{ *static_cast<T*>(src) };
		}

		void copy(void *object, void *src) const override {
			*static_cast<T*>(object) = *static_cast<T*>(src);
		}

		void destruct(void *object) const override {
			static_cast<T*>(object)->~T();
		}

		void serialize(Puper &puper, void *data, const ObjInfo &info) const override {
			pup(puper, *static_cast<T*>(data), info);
		}
	};

}
