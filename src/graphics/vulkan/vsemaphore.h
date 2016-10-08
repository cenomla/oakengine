#pragma once

#include <vulkan/vulkan.h>

#include "vdevice.h"

namespace oak::graphics {

	class VSemaphore {
	public:
		VSemaphore(const VDevice &device);
		~VSemaphore();

		void create();
		void destroy();

		inline operator const VkSemaphore() const { return semaphore_; }
	private:
		const VDevice &device_;
		VkSemaphore semaphore_;
	};

}