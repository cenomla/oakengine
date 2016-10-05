#pragma once

#include <vulkan/vulkan.h>

#include "vdevice.h"

namespace oak::graphics {

	class VMemory {
	public:
		VMemory(const VDevice *device);
		~VMemory();

		void allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties);
		void free();

		void map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** data);
		void unmap();

		void bindImage(VkImage image, VkDeviceSize offset);
		void bindBuffer(VkBuffer buffer, VkDeviceSize offset);
	private:
		const VDevice *device_;
		VkDeviceMemory mem_;

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};

}