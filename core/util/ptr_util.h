#pragma once

#include "types.h"

namespace oak::ptrutil {

	constexpr inline size_t align_size(size_t size, uint32_t alignment) {
		return (size + alignment-1) & (~(alignment-1));
	}

	constexpr inline uint32_t align_offset(const void *address, uint32_t alignment) {
		uint32_t adjustment = (alignment - (reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1)));
		return adjustment == alignment ? 0 : adjustment;
	}

	constexpr inline void* align_address(const void *address, uint32_t alignment) {
		return add(address, align_offset(address, alignment));
	}

	constexpr inline uint32_t align_offset_with_header(const void *address, uint32_t alignment, uint32_t headerSize) {
		uint32_t adjustment = align_offset(address, alignment);

		uint32_t neededSpace = headerSize;

		if (adjustment < neededSpace) {
			neededSpace -= adjustment;

			adjustment += alignment * (neededSpace / alignment);

			if (neededSpace % alignment > 0) {
				adjustment += alignment;
			}
		}
		return adjustment;
	}

	constexpr inline void* add(void* p, size_t x) {
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(p) + x);
	}

	constexpr inline const void* add(const void* p, size_t x) {
		return reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(p) + x);
	}

	constexpr inline void* subtract(void* p, size_t x) {
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(p) - x);
	}

	constexpr inline const void* subtract(const void* p, size_t x) {
		return reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(p) - x);
	}
}
