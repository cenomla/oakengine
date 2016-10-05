#pragma once

#include <iostream>
#include <vulkan/vulkan.h>

#include "vdevice.h"

namespace oak::graphics {

	class VShader {
	public:
		VShader(const VDevice *device, VkShaderStageFlagBits stage);
		~VShader();

		VShader(VShader &&other);

		void load(const std::string &path);
		void destroy();

		operator const VkPipelineShaderStageCreateInfo() const;
	private:
		const VDevice *device_;
		VkShaderModule module_;
		VkShaderStageFlagBits stage_;
	};

}