#pragma once

#include <vulkan/vulkan.h>
#include "vdevice.h"

namespace oak::graphics {

	class VImage {
	public:
		VImage(const VDevice &device);
		~VImage();

		void create(size_t width, size_t height, VkFormat format, VkImageTiling tiling, VkImageLayout layout, VkImageUsageFlags usage);
		void createView(VkImageAspectFlags aspect);
		void destroy();

		inline operator const VkImage() const { return image_; }
		inline operator const VkImageView() const { return view_; }

		VkMemoryRequirements getMemoryRequirements() const;
	private:
		const VDevice &device_;
		VkImage image_;
		VkImageView view_;
		VkFormat format_;
	};

}