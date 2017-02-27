#include "vdescriptor_pool.h"

#include "log.h"

namespace oak::graphics {

	VDescriptorPool::VDescriptorPool(const VDevice &device) : device_{ device }, pool_{ VK_NULL_HANDLE } {}

	VDescriptorPool::~VDescriptorPool() {
		destroy();
	}

	void VDescriptorPool::create(const oak::vector<VkDescriptorPoolSize> &poolSizes) {
		if (pool_ != VK_NULL_HANDLE) { return; }

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;

		VkResult result = vkCreateDescriptorPool(device_, &poolInfo, nullptr, &pool_);
		if (result != VK_SUCCESS) {
			log::cout << "failed to create descriptor pool" << std::endl;
			abort();
		}
	}

	void VDescriptorPool::destroy() {
		if (pool_ != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(device_, pool_, nullptr);
			pool_ = VK_NULL_HANDLE;
		}
	}

}