#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "vdevice.h"

namespace oak::graphics {

	class VDescriptorPool {
	public:
		VDescriptorPool(const VDevice &device);
		~VDescriptorPool();

		void create(const std::vector<VkDescriptorPoolSize> &poolSizes);
		void destroy();

		inline operator const VkDescriptorPool() const { return pool_; }
	private:
		const VDevice &device_;
		VkDescriptorPool pool_;

	};

}