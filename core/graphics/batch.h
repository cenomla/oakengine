#pragma once

#include <cstddef>
#include <cinttypes>

namespace oak::graphics {

	class BufferStorage;
	struct Material;

	struct Batch {
		static constexpr uint32_t INDEX = 0x01;
		static constexpr uint32_t DRAW_MASK = 0xFE;
		static constexpr uint32_t DRAW_POINTS = 0x01 << 1;
		static constexpr uint32_t DRAW_LINES = 0x02 << 1;
		static constexpr uint32_t DRAW_LINE_LOOP = 0x03 << 1;
		static constexpr uint32_t DRAW_LINE_STRIP = 0x04 << 1;
		static constexpr uint32_t DRAW_TRIANGLES = 0x05 << 1;
		static constexpr uint32_t DRAW_TRIANGLE_STRIP = 0x06 << 1;
		static constexpr uint32_t DRAW_TRIANGLE_FAN = 0x07 << 1;

		const BufferStorage *storage = nullptr;
		const Material *material = nullptr;
		size_t offset = 0, count = 0;
		uint32_t layer = 0;
		uint32_t flags = INDEX | DRAW_TRIANGLES;
		int instances = -1;
	};

}