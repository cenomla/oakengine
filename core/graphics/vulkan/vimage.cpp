#include "vimage.h"

#include "log.h"

namespace oak::graphics {

	VImage::VImage(const VDevice &device) : device_{ device }, image_{ VK_NULL_HANDLE }, view_{ VK_NULL_HANDLE } {

	}

	VImage::~VImage() {
		destroy();
	}

	void VImage::create(size_t width, size_t height, VkFormat format, VkImageTiling tiling, VkImageLayout layout, VkImageUsageFlags usage) {
		if (image_ == VK_NULL_HANDLE) {
			format_ = format;
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format_;
			imageInfo.tiling = tiling;
			imageInfo.initialLayout = layout;
			imageInfo.usage = usage;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.flags = 0;

			VkResult result = vkCreateImage(device_, &imageInfo, nullptr, &image_);
			if (result != VK_SUCCESS) {
				log::cout << "failed to create image" << std::endl;
				abort();
			}
		}

		
	}

	void VImage::createView(VkImageAspectFlags aspect) {
		if (view_ == VK_NULL_HANDLE) {
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image_;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = format_;
			viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.subresourceRange.aspectMask = aspect;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VkResult result = vkCreateImageView(device_, &viewInfo, nullptr, &view_);
			if (result != VK_SUCCESS) {
				log::cout << "failed to create image view" << std::endl;
				abort();
			}
		}
	}

	void VImage::destroy() {
		if (image_ != VK_NULL_HANDLE) {
			vkDestroyImage(device_, image_, nullptr);
			image_ = VK_NULL_HANDLE;
		}
		if (view_ != VK_NULL_HANDLE) {
			vkDestroyImageView(device_, view_, nullptr);
			view_ = VK_NULL_HANDLE;
		}
	}

	VkMemoryRequirements VImage::getMemoryRequirements() const {
		VkMemoryRequirements requirement;
		vkGetImageMemoryRequirements(device_, image_, &requirement);
		return requirement;
	}

}