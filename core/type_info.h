#pragma once

#include "util/type_id.h"
#include "container.h"
#include "pup.h"

namespace oak {

	namespace detail {
		template<typename T>
		struct has_destroy_method {
			template<typename U, void (U::*)()> struct SFINAE {};
			template<typename U> static char Test(SFINAE<U, &U::destroy>*);
			template<typename U> static int Test(...);
			static constexpr bool value = sizeof(Test<T>(0)) == sizeof(char);
		};

		template<class T>
		void construct(void *object) {
			new (object) T{};
		}
		
		template<class T>
		void copyConstruct(void *object, const void *src) {
			new (object) T{ *static_cast<const T*>(src) };
		}

		template<class T>
		void copy(void *object, const void *src) {
			*static_cast<T*>(object) = *static_cast<const T*>(src);
		}

		template<class T>
		void destroy(void *object) {
			if constexpr(has_destroy_method<T>::value) {
				static_cast<T*>(object)->destroy();
			}
		}

		template<class T>
		void destruct(void *object) {
			static_cast<T*>(object)->~T();
		}

		template<class T>
		void serialize(Puper& puper, void *data, const ObjInfo& parent, const oak::string& name) {
			pup(puper, *static_cast<T*>(data), ObjInfo::make<T>(&parent, name));
		}
	}

	struct TypeInfo {
		oak::string name;
		size_t size;
		size_t id;
		
		void (*construct)(void *object);
		void (*copyConstruct)(void *object, const void *src);
		void (*copy)(void *object, const void *src);
		void (*destroy)(void *object);
		void (*destruct)(void *object);
		void (*serialize)(Puper& puper, void *data, const ObjInfo& parent, const oak::string& name);
	};

	template<class U, class T>
	TypeInfo makeTypeInfo(const oak::string& name) {
		return TypeInfo {
			name,
			sizeof(T),
			util::type_id<U, T>::id(),
			detail::construct<T>,
			detail::copyConstruct<T>,
			detail::copy<T>,
			detail::destroy<T>,
			detail::destruct<T>,
			detail::serialize<T>
		};
	}

}
