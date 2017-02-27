#pragma once

#include <vulkan/vulkan.h>

#include "memory/container.h"
#include "vdevice.h"

namespace oak::graphics {

	class VDescriptorPool {
	public:
		VDescriptorPool(const VDevice &device);
		~VDescriptorPool();

		void create(const oak::vector<VkDescriptorPoolSize> &poolSizes);
		void destroy();

		inline operator const VkDescriptorPool() const { return pool_; }
	private:
		const VDevice &device_;
		VkDescriptorPool pool_;

	};

}