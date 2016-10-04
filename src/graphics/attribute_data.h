#pragma once

#include <cinttypes>

namespace oak::graphics {

	constexpr uint32_t ATTRIB_FORMAT_1F = 0;
	constexpr uint32_t ATTRIB_FORMAT_2F = 1;
	constexpr uint32_t ATTRIB_FORMAT_3F = 2;
	constexpr uint32_t ATTRIB_FORMAT_4F = 3;

	constexpr uint32_t ATTRIB_FORMAT_1B = 4;
	constexpr uint32_t ATTRIB_FORMAT_2B = 5;
	constexpr uint32_t ATTRIB_FORMAT_3B = 6;
	constexpr uint32_t ATTRIB_FORMAT_4B = 7;

	struct BindingData {
		uint32_t binding;
		uint32_t vertexInput;
		size_t stride;
	};

	struct AttributeData {
		uint32_t binding;
		uint32_t location;
		uint32_t format;
		size_t offset;
	};

}