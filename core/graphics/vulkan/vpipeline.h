#pragma once

#include <initializer_list>
#include <vulkan/vulkan.h>

#include "memory/container.h"
#include "graphics/attribute_data.h"
#include "vdevice.h"
#include "vshader.h"

namespace oak::graphics {

	class VPipeline {
	public:
		VPipeline(const VDevice &device);
		~VPipeline();

		void addBindingDescription(BindingData &&description);
		void addAttributeDescription(std::initializer_list<AttributeData> &&description);
		void addShaderModule(VShader &&shader);
		void create(VkDescriptorSetLayout descriptorSetLayout, VkExtent2D extent, VkRenderPass renderPass, uint32_t subpassIndex);
		void destroy();

		inline operator const VkPipeline() const { return pipeline_; }

		inline const VkPipelineLayout getLayout() const { return layout_; }
	private:
		const VDevice &device_;
		VkPipelineLayout layout_;
		VkPipeline pipeline_;
		oak::vector<BindingData> bindingDescriptions_;
		oak::vector<AttributeData> attributeDescriptions_;
		oak::vector<VShader> modules_;
	};

}