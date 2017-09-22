#pragma once

#include <cstddef>

#include "type_info.h"

namespace oak {

	namespace config {
		constexpr size_t MAX_EVENTS = 128;
	}

	namespace detail {
		struct BaseEvent {};
	}

	template<class T>
	TypeInfo makeEventInfo(const oak::string& name) {
		return makeTypeInfo<detail::BaseEvent, T>(name);
	}
}
