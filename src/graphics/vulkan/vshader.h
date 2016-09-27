#pragma once

#include <iostream>
#include <vulkan/vulkan.h>

namespace oak::graphics {

	class VShader {
	public:
		VShader(VkDevice device, VkShaderStageFlagBits stage);
		~VShader();

		VShader(VShader &&other);

		void load(const std::string &path);
		void destroy();

		operator const VkPipelineShaderStageCreateInfo() const;
	private:
		VkDevice device_;
		VkShaderModule module_;
		VkShaderStageFlagBits stage_;
	};

}