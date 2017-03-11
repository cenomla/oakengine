#pragma once

#include <cinttypes>
#include <cstddef>

namespace oak::ptrutil {

	constexpr inline size_t alignSize(size_t size, uint32_t alignment = 8) {
		return (size + alignment-1) & (~(alignment-1));
	}

	constexpr inline void* alignForward(const void *address, uint32_t alignment) {
		return reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
	}

	constexpr inline uint32_t alignForwardAdjustment(const void *address, uint32_t alignment) {
		uint32_t adjustment = (alignment - (reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1)));
		return adjustment == alignment ? 0 : adjustment;
	}

	constexpr inline uint32_t alignForwardAdjustmentWithHeader(const void *address, uint32_t alignment, uint32_t headerSize) {
		uint32_t adjustment = alignForwardAdjustment(address, alignment);

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