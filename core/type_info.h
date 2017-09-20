#pragma once

#include "util/type_id.h"
#include "container.h"
#include "component.h"
#include "events.h"
#include "pup.h"

namespace oak {

	struct TypeInfo {
		oak::string name;
		size_t size;
		size_t id;
		
		void (*construct)(void *object);
		void (*copyConstruct)(void *object, const void *src);
		void (*copy)(void *object, const void *src);
		void (*destruct)(void *object);
		void (*serialize)(Puper& puper, void *data, const ObjInfo& parent, const oak::string& name);
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
	void destruct(void *object) {
		static_cast<T*>(object)->~T();
	}

	template<class T>
	void serialize(Puper& puper, void *data, const ObjInfo& parent, const oak::string& name) {
		pup(puper, *static_cast<T*>(data), ObjInfo::make<T>(&parent, name));
	}

	template<class U, class T>
	TypeInfo makeTypeInfo(const oak::string& name) {
		return TypeInfo {
			name,
			sizeof(T),
			util::type_id<U, T>::id(),
			construct<T>,
			copyConstruct<T>,
			copy<T>,
			destruct<T>,
			serialize<T>
		};
	}

	template<class T>
	TypeInfo makeComponentInfo(const oak::string& name) {
		return makeTypeInfo<oak::detail::BaseComponent, T>(name);
	}

	template<class T>
	TypeInfo makeEventInfo(const oak::string& name) {
		return makeTypeInfo<oak::detail::BaseEvent, T>(name);
	}


}
