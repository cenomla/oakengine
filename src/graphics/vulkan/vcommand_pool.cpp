#include "vcommand_pool.h"

#include "log.h"

namespace oak::graphics {

	VCommandPool::VCommandPool(const VDevice *device) : device_{ device }, pool_{ VK_NULL_HANDLE } {

	}

	VCommandPool::~VCommandPool() {
		destroy();
	}

	void VCommandPool::create(int queueIndex) {
		if (pool_ != VK_NULL_HANDLE) { return; }

		VkCommandPoolCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.queueFamilyIndex = queueIndex;
		info.flags = 0;

		VkResult result = vkCreateCommandPool(*device_, &info, nullptr, &pool_);
		if (result != VK_SUCCESS) {
			log::cout << "failed to create command pool" << std::endl;
			std::exit(-1);
		}
	}

	void VCommandPool::destroy() {
		if (pool_ != VK_NULL_HANDLE) {
			vkDestroyCommandPool(*device_, pool_, nullptr);
			pool_ = VK_NULL_HANDLE;
		}
	}

	void VCommandPool::allocateBuffers(size_t count, VkCommandBuffer *buffers, VkCommandBufferLevel level) {
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = pool_;
		allocInfo.level = level;
		allocInfo.commandBufferCount = static_cast<uint32_t>(count);

		VkResult result = vkAllocateCommandBuffers(*device_, &allocInfo, buffers);
		if (result != VK_SUCCESS) {
			log::cout << "failed to allocate command buffers" << std::endl;
			std::exit(-1);
		}
	}

	void VCommandPool::freeBuffers(size_t count, VkCommandBuffer *buffers) {
		vkFreeCommandBuffers(*device_, pool_, count, buffers);
	}

}