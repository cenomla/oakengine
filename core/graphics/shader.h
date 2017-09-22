#pragma once

#include <cinttypes>
#include "resource.h"

namespace oak::graphics {

	struct ShaderInfo {
		const char *vertex = nullptr;
		const char *fragment = nullptr;
		const char *geometry = nullptr;
		const char *tessellation = nullptr;
	};

	struct Shader {
		static const TypeInfo typeInfo;

		uint32_t id = 0;
		ShaderInfo info;

		void destroy();
	};

	void pup(Puper& puper, Shader& data, const ObjInfo& info);

}
