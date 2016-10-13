#pragma once

#include <vulkan/vulkan.h>

#include "vdevice.h"

namespace oak::graphics {

	class VBuffer {
	public:
		VBuffer(const VDevice &device);
		~VBuffer();

		void create(VkDeviceSize size, VkBufferUsageFlags usage);
		void destroy();

		inline operator const VkBuffer() const { return buffer_; }

		VkMemoryRequirements getMemoryRequirements() const;
	private:
		const VDevice &device_;
		VkBuffer buffer_;
	};

}