#pragma once

#include <cstddef>
#include <cinttypes>

#include "graphics/api.h"

namespace oak::graphics {


	struct BufferInfo {
		BufferType type;
		BufferHint hint = BufferHint::STATIC;
		int base = -1;
	};

	struct Buffer {
		uint32_t id = 0;
		BufferInfo info;
		
		void destroy();
	};

}