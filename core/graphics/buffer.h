#pragma once

#include <cstddef>
#include <cinttypes>

#include "api.h"
#include "resource.h"

namespace oak::graphics {


	struct BufferInfo {
		BufferType type;
		BufferHint hint = BufferHint::STATIC;
		int base = -1;
	};

	struct Buffer {
		static const TypeInfo typeInfo;

		uint32_t id = 0;
		BufferInfo info;

		void destroy();
	};

	void pup(Puper& puper, Buffer& data, const ObjInfo& info);

}
