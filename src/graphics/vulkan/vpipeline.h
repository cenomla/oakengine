#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "vshader.h"

namespace oak::graphics {

	class VPipeline {
	public:
		VPipeline(const VkDevice *device);
		~VPipeline();

		void addBindingDescription(VkVertexInputBindingDescription description);
		void addAttributeDescription(VkVertexInputAttributeDescription description);
		void addShaderModule(VShader &&shader);
		void create(VkDescriptorSetLayout descriptorSetLayout, VkExtent2D extent, VkRenderPass renderPass, uint32_t subpassIndex);
		void destroy();

		inline operator const VkPipeline() const { return pipeline_; }

		inline const VkPipelineLayout getLayout() const { return layout_; }
	private:
		const VkDevice *device_;
		VkPipelineLayout layout_;
		VkPipeline pipeline_;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions_;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions_;
		std::vector<VShader> modules_;
	};

}