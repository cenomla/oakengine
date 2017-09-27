#pragma once

#include <cstddef>

#include "type_manager.h"

namespace oak {

	namespace detail {
		struct BaseEvent {};
	}

	template<class T>
	TypeInfo makeEventInfo(const oak::string& name) {
		return makeTypeInfo<detail::BaseEvent, T>(name);
	}

	using EventTypeManager = TypeManager<detail::BaseEvent>;
}
