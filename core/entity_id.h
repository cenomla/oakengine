#pragma once

namespace oak {

	namespace config {
		constexpr size_t MAX_COMPONENTS = 64;
	}

	namespace detail {
		struct BaseComponent {};
	}


	struct EntityId {
		uint32_t index = 0;
		uint32_t generation = 0;

		operator uint32_t() const { return index; }
	};

}