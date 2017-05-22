#pragma once

#include "util/type_id.h"
#include "container.h"
#include "pup.h"

namespace oak {

	struct TypeHandleBase {
		TypeHandleBase(const oak::string &name, size_t size) : name_{ name }, size_{ size } {};
		virtual ~TypeHandleBase() {};
		virtual void construct(void *object) const = 0;
		virtual void construct(void *object, const void *src) const = 0;
		virtual void copy(void *object, const void *src) const = 0;
		virtual void destruct(void *object) const = 0;
		virtual void serialize(Puper &puper, void *data, const ObjInfo &info) const = 0;
		virtual size_t getId() const = 0;
		size_t getSize() const { return size_; }
		const oak::string& getName() const { return name_; }
	protected:
		oak::string name_;
		size_t size_;
	};

	template <class U, class T>
	struct TypeHandle : public TypeHandleBase {
		TypeHandle(const oak::string &name) : TypeHandleBase{ name, sizeof(T) } {};

		void construct(void *object) const override {
			new (object) T{};
		}

		void construct(void *object, const void *src) const override {
			new (object) T{ *static_cast<const T*>(src) };
		}

		void copy(void *object, const void *src) const override {
			*static_cast<T*>(object) = *static_cast<const T*>(src);
		}

		void destruct(void *object) const override {
			static_cast<T*>(object)->~T();
		}

		void serialize(Puper &puper, void *data, const ObjInfo &info) const override {
			pup(puper, *static_cast<T*>(data), info);
		}

		size_t getId() const override {
			return util::type_id<U, T>::id;
		}
	};

}
