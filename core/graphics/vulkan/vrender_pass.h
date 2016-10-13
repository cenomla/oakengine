#pragma once

#include <vulkan/vulkan.h>

namespace oak::graphics {

	class VDevice;

	class VRenderPass {
	public:
		VRenderPass(const VDevice &device);
		~VRenderPass();
	
		void create(VkFormat colorFormat, VkFormat depthFormat);
		void destroy();

		inline operator const VkRenderPass() const { return renderPass_; }
	private:
		const VDevice &device_;
		VkRenderPass renderPass_;
	};

}