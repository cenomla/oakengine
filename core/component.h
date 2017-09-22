#pragma once

#include <cstddef>

#include "type_info.h"

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


}
