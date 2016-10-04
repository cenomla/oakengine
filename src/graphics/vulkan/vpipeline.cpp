#include "vpipeline.h"

#include "log.h"

namespace oak::graphics {

	constexpr VkFormat formats[] = {
		VK_FORMAT_R32_SFLOAT,
		VK_FORMAT_R32G32_SFLOAT,
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32A32_SFLOAT
	};

	VPipeline::VPipeline(const VkDevice *device) : device_{ device }, layout_{ VK_NULL_HANDLE }, pipeline_{ VK_NULL_HANDLE } {

	}

	VPipeline::~VPipeline() {
		destroy();
	}

	void VPipeline::addBindingDescription(BindingData &&description) {
		bindingDescriptions_.push_back(description);
	}

	void VPipeline::addAttributeDescription(std::initializer_list<AttributeData> &&description) {
		for (auto &&data : description) {
			attributeDescriptions_.push_back(data);
		}
	}

	void VPipeline::addShaderModule(VShader &&shader) {
		modules_.emplace_back(std::move(shader));
	}

	void VPipeline::create(VkDescriptorSetLayout descriptorSetLayout, VkExtent2D extent, VkRenderPass renderPass, uint32_t subPassIndex) {
		//create the pipeline layout (never create it twice)
		if (layout_ == VK_NULL_HANDLE) {
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.setLayoutCount = 1;
			pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
			pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
			pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

			VkResult result = vkCreatePipelineLayout(*device_, &pipelineLayoutCreateInfo, nullptr, &layout_);
			if (result != VK_SUCCESS) {
				log::cout << "couldnt create a pipeline layout" << std::endl;
				std::exit(-1);
			}
		}

		//if the pipeline already exists then destroy it so that we dont leak resources
		if (pipeline_ != VK_NULL_HANDLE) {
			vkDestroyPipeline(*device_, pipeline_, nullptr);
			pipeline_ = VK_NULL_HANDLE;
		}

		//get shader module create infos
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		for (const auto &module : modules_) {
			shaderStages.push_back(module);
		}

		std::vector<VkVertexInputBindingDescription> bindings;
		bindings.reserve(bindingDescriptions_.size());
		for (const auto &description : bindingDescriptions_) {
			VkVertexInputBindingDescription binding;
			binding.binding = description.binding;
			binding.stride = description.stride;
			binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindings.push_back(binding);
		}

		std::vector<VkVertexInputAttributeDescription> attribs;
		attribs.reserve(attributeDescriptions_.size());
		for (const auto &description : attributeDescriptions_) {
			VkVertexInputAttributeDescription attrib;
			attrib.binding = description.binding;
			attrib.location = description.location;
			attrib.format = formats[description.format];
			attrib.offset = description.offset;
			attribs.push_back(attrib);
		}

		VkPipelineVertexInputStateCreateInfo vertInputInfo = {};
		vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertInputInfo.vertexBindingDescriptionCount = bindings.size();
		vertInputInfo.pVertexBindingDescriptions = bindings.data();
		vertInputInfo.vertexAttributeDescriptionCount = attribs.size();
		vertInputInfo.pVertexAttributeDescriptions = attribs.data();

		//fixed function

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = extent.width;
		viewport.height = extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = extent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_R_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = layout_;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = subPassIndex;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		VkResult result = vkCreateGraphicsPipelines(*device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline_);
		if (result != VK_SUCCESS) {
			log::cout << "couldnt create the graphics pipeline" << std::endl;
			std::exit(-1);
		}



	}

	void VPipeline::destroy() {
		if (pipeline_ != VK_NULL_HANDLE) {
			vkDestroyPipeline(*device_, pipeline_, nullptr);
			pipeline_ = VK_NULL_HANDLE;
		}
		if (layout_ != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(*device_, layout_, nullptr);
			layout_ = nullptr;
		}

		bindingDescriptions_.clear();
		attributeDescriptions_.clear();
		modules_.clear();
	}


}