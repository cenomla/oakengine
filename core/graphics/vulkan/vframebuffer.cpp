#include "vframebuffer.h"

#include "log.h"

namespace oak::graphics {

	VFramebuffer::VFramebuffer(const VDevice *device) : device_{ device }, framebuffer_{ VK_NULL_HANDLE } {

	}

	VFramebuffer::~VFramebuffer() {
		destroy();
	}

	void VFramebuffer::create(oak::vector<VkImageView> &&imageViews, VkRenderPass renderPass, size_t width, size_t height) {
		if (framebuffer_ != VK_NULL_HANDLE) { return; }

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = imageViews.size();
		framebufferInfo.pAttachments = imageViews.data();
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(*device_, &framebufferInfo, nullptr, &framebuffer_);
		if (result != VK_SUCCESS) {
			log::cout << "failed create a framebuffer" << std::endl;
			abort();
		}
	}

	void VFramebuffer::destroy() {
		if (framebuffer_ != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(*device_, framebuffer_, nullptr);
			framebuffer_ = VK_NULL_HANDLE;
		}
	}
	
}