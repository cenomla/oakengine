#pragma once

namespace oak {

	struct TypeHandleBase {
		TypeHandleBase(size_t size) : size_{ size } {};
		virtual ~TypeHandleBase() {};
		virtual void construct(void *src, void *target) const = 0;
		virtual void copy(void *src, void *target) const = 0;
		virtual void destruct(void *comp) const = 0;
		inline size_t size() const { return size_; }
	protected:
		size_t size_;
	};

	template <class T>
	struct TypeHandle : public TypeHandleBase {
		TypeHandle() : TypeHandleBase{ sizeof(T) } {};

		void construct(void *src, void *target) const {
			new (target) T{ *static_cast<T*>(src) };
		}

		void copy(void *src, void *target) const override {
			*static_cast<T*>(target) = *static_cast<T*>(src);
		}

		void destruct(void *comp) const override {
			static_cast<T*>(comp)->~T();
		}
	};

}
