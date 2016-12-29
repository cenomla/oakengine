#pragma once

#include <string>

#include "util/puper.h"

namespace oak {

	struct TypeHandleBase {
		TypeHandleBase(const std::string &name, size_t size) : name_{ name }, size_{ size } {};
		virtual ~TypeHandleBase() {};
		virtual void construct(void *object) const = 0;
		virtual void construct(void *src, void *target) const = 0;
		virtual void copy(void *src, void *target) const = 0;
		virtual void destruct(void *comp) const = 0;
		virtual void pupObject(util::Puper &puper, void *data, const util::ObjInfo &info) const = 0;
		inline size_t size() const { return size_; }
		inline const std::string& name() const { return name_; }
	protected:
		std::string name_;
		size_t size_;
	};

	template <class T>
	struct TypeHandle : public TypeHandleBase {
		TypeHandle(const std::string &name) : TypeHandleBase{ name, sizeof(T) } {};

		void construct(void *object) const {
			new (object) T{};
		}

		void construct(void *src, void *target) const {
			new (target) T{ *static_cast<T*>(src) };
		}

		void copy(void *src, void *target) const override {
			*static_cast<T*>(target) = *static_cast<T*>(src);
		}

		void destruct(void *comp) const override {
			static_cast<T*>(comp)->~T();
		}

		void pupObject(util::Puper &puper, void *data, const util::ObjInfo &info) const override {
			using namespace util;
			pup(puper, *static_cast<T*>(data), info);
		}
	};

}
