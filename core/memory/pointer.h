#pragma once

#include "block.h"

namespace oak { namespace ptrmath {

	constexpr inline size_t alignSize(size_t size, byte alignment = 8) {
		return (size + alignment-1) & (~(alignment-1));
	}

	inline void* alignForward(void *address, byte alignment) {
		return reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
	}

	inline byte alignForwardAdjustment(const void *address, byte alignment) {
		byte adjustment = alignment - (reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1));

		if (adjustment == alignment) { return 0; }

		return adjustment;
	}

	inline byte alignForwardAdjustmentWithHeader(const void *address, byte alignment, byte headerSize) {
		byte adjustment = alignForwardAdjustment(address, alignment);

		byte neededSpace = headerSize;

		if (adjustment < neededSpace) {
			neededSpace -= adjustment;

			adjustment += alignment * (neededSpace / alignment);

			if (neededSpace % alignment > 0) {
				adjustment += alignment;
			}
		}
		return adjustment;
	}

	inline void* add(void* p, size_t x) {
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(p) + x);
	}

	inline const void* add(const void* p, size_t x) {
		return reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(p) + x);
	}

	inline void* subtract(void* p, size_t x) {
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(p) - x);
	}

	inline const void* subtract(const void* p, size_t x) {
		return reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(p) - x);
	}
} }