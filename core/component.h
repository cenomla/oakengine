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

	using ComponentTypeManager = TypeManager<detail::BaseComponent>;

}
