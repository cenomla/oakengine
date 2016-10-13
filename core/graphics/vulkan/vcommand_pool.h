#pragma once

#include <vulkan/vulkan.h>

#include "vdevice.h"

namespace oak::graphics {

	class VCommandPool {
	public:
		
		VCommandPool(const VDevice &device);
		~VCommandPool();

		void create(int queueIndex);
		void destroy();

		void allocateBuffers(size_t count, VkCommandBuffer *buffers, VkCommandBufferLevel level);
		void freeBuffers(size_t count, VkCommandBuffer *buffers);


	private:
		const VDevice &device_;
		VkCommandPool pool_;
	};

}