#include "vsemaphore.h"

#include "log.h"

namespace oak::graphics {

	VSemaphore::VSemaphore(const VDevice &device) : device_{ device }, semaphore_{ VK_NULL_HANDLE } {

	}

	VSemaphore::~VSemaphore() {
		destroy();
	}

	void VSemaphore::create() {
		if (semaphore_ != VK_NULL_HANDLE) { return; }

		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkResult result = vkCreateSemaphore(device_, &info, nullptr, &semaphore_);
		if (result != VK_SUCCESS) {
			log::cout << "failed to create semaphore" << std::endl;
			abort();
		}
	}

	void VSemaphore::destroy() {
		if (semaphore_ != VK_NULL_HANDLE) {
			vkDestroySemaphore(device_, semaphore_, nullptr);
			semaphore_ = VK_NULL_HANDLE;
		}
	}

}