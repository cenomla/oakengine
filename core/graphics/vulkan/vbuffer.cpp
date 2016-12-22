#include "vbuffer.h"

#include "log.h"

namespace oak::graphics {

	VBuffer::VBuffer(const VDevice &device) : device_{ device }, buffer_{ VK_NULL_HANDLE } {

	}

	VBuffer::~VBuffer() {
		destroy();
	}

	void VBuffer::create(VkDeviceSize size, VkBufferUsageFlags usage) {
		if (buffer_ != VK_NULL_HANDLE) { return; }

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer_);
		if (result != VK_SUCCESS) {
			log::cout << "failed to create vertex buffer" << std::endl;
			abort();
		}
	}

	void VBuffer::destroy() {
		if (buffer_ != VK_NULL_HANDLE) {
			vkDestroyBuffer(device_, buffer_, nullptr);
			buffer_ = VK_NULL_HANDLE;
		}
	}

	VkMemoryRequirements VBuffer::getMemoryRequirements() const {
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device_, buffer_, &requirements);
		return requirements;
	}

}