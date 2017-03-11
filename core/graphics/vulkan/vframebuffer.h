#pragma once

#include <vulkan/vulkan.h>

#include "container.h"
#include "vdevice.h"

namespace oak::graphics {

	class VFramebuffer {
	public:
		VFramebuffer(const VDevice *device);
		~VFramebuffer();

		void create(oak::vector<VkImageView> &&views, VkRenderPass renderPass, size_t width, size_t height);
		void destroy();

		inline operator const VkFramebuffer() const { return framebuffer_; }

	private:
		const VDevice *device_;
		VkFramebuffer framebuffer_;
	};

}