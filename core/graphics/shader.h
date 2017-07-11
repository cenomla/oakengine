#pragma once

#include <cinttypes>

namespace oak::graphics {

	struct ShaderInfo {
		const char *vertex = nullptr;
		const char *fragment = nullptr;
		const char *geometry = nullptr;
		const char *tessellation = nullptr;
	};

	struct Shader {
		uint32_t id = 0;
		ShaderInfo info;
	};

}