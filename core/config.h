#pragma once

#include <cinttypes>
#include <cstddef>

namespace oak::config {

	constexpr size_t MAX_COMPONENTS = 64;
	constexpr int GL_VERSION_MAJOR = 4;
	constexpr int GL_VERSION_MINOR = 3;
	constexpr size_t GLOBAL_MEMORY_PAGE_SIZE = 64e6;
	constexpr size_t FRAME_MEMORY_PAGE_SIZE = 8e6;
	constexpr size_t DEFAULT_MEMORY_ALIGNMENT = 8;

}