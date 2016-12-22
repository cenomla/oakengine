#include "vmemory.h"

#include <limits>

#include "log.h"

namespace oak::graphics {

	VMemory::VMemory(const VDevice &device) : device_{ device }, mem_{ VK_NULL_HANDLE } {

	}

	VMemory::~VMemory() {
		free();
	}

	void VMemory::allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties) {
		if (mem_ != VK_NULL_HANDLE) { return; }

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = requirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(requirements.memoryTypeBits, properties);

		VkResult result = vkAllocateMemory(device_, &allocInfo, nullptr, &mem_);
		if (result != VK_SUCCESS) {
			log::cout << "failed to allocate image memory" << std::endl;
			abort();
		}
	}

	void VMemory::free() {
		if (mem_ != VK_NULL_HANDLE) {
			vkFreeMemory(device_, mem_, nullptr);
			mem_ = VK_NULL_HANDLE;
		}
	}

	void VMemory::map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **data) {
		vkMapMemory(device_, mem_, offset, size, flags, data);
	}

	void VMemory::unmap() {
		vkUnmapMemory(device_, mem_);
	}

	void VMemory::bindImage(VkImage image, VkDeviceSize offset) {
		vkBindImageMemory(device_, image, mem_, offset);
	}

	void VMemory::bindBuffer(VkBuffer buffer, VkDeviceSize offset) {
		vkBindBufferMemory(device_, buffer, mem_, offset);
	}

	uint32_t VMemory::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device_, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		return std::numeric_limits<uint32_t>::max();
	}

}