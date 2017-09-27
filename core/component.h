#pragma once

#include <cstddef>

#include "type_manager.h"

namespace oak {

	namespace config {
		constexpr size_t MAX_COMPONENTS = 64;
	}

	namespace detail {
		struct BaseComponent {};
	}

	template<class T>
	TypeInfo makeComponentInfo(const oak::string& name) {
		return makeTypeInfo<detail::BaseComponent, T>(name);
	}

	using ComponentTypeManager = TypeManager<detail::BaseComponent>;

}
