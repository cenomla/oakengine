#pragma once

#include <cinttypes>
#include <cstddef>

namespace oak {

	struct ObjInfo;
	class Puper;

	struct EntityId {
		uint32_t index = 0;
		uint32_t generation = 0;

		operator uint32_t() const { return index; }
	};

	void pup(Puper& puper, EntityId& data, const ObjInfo& info);

}