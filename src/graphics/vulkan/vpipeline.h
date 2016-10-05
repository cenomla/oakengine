#pragma once

#include <initializer_list>
#include <vector>
#include <vulkan/vulkan.h>

#include "graphics/attribute_data.h"
#include "vdevice.h"
#include "vshader.h"

namespace oak::graphics {

	class VPipeline {
	public:
		VPipeline(const VDevice *device);
		~VPipeline();

		void addBindingDescription(BindingData &&description);
		void addAttributeDescription(std::initializer_list<AttributeData> &&description);
		void addShaderModule(VShader &&shader);
		void create(VkDescriptorSetLayout descriptorSetLayout, VkExtent2D extent, VkRenderPass renderPass, uint32_t subpassIndex);
		void destroy();

		inline operator const VkPipeline() const { return pipeline_; }

		inline const VkPipelineLayout getLayout() const { return layout_; }
	private:
		const VDevice *device_;
		VkPipelineLayout layout_;
		VkPipeline pipeline_;
		std::vector<BindingData> bindingDescriptions_;
		std::vector<AttributeData> attributeDescriptions_;
		std::vector<VShader> modules_;
	};

}