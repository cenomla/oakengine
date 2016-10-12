#pragma once

#include <bitset>

namespace oak::graphics {

	struct Material {

		uint32_t id;
		//pipeline descriptor
		uint32_t shaderId;
		uint32_t textureId;
		std::bitset<32> flags;
	};

}