#include "api.h"

#include <cstring>
#include <limits>
#include <chrono>
#include <set>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "util/file_util.h"
#include "graphics/vertex.h"
#include "vshader.h"

#include "log.h"

namespace oak::graphics {

	VulkanApi::VulkanApi() : instance_{ [](VkInstance instance) { vkDestroyInstance(instance, nullptr); } }, 
		debugReport_{ [this](VkDebugReportCallbackEXT callback) { 
			auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_, "vkDestroyDebugReportCallbackEXT"));
			func(this->instance_, callback, nullptr);
		} },
		surface_{ [this](VkSurfaceKHR surface) { vkDestroySurfaceKHR(instance_, surface, nullptr); } },
		device_{ &instance_ },
		swapchain_{ [this](VkSwapchainKHR swapchain) { vkDestroySwapchainKHR(device_, swapchain, nullptr); } },
		renderPass_{ device_ },
		descriptorSetLayout_{ [this](VkDescriptorSetLayout layout) { vkDestroyDescriptorSetLayout(device_, layout, nullptr); } },
		graphicsPipeline_{ device_ },
		depthImage_{ device_ },
		depthImageMemory_{ device_ },
		textureImage_{ device_ },
		textureImageMemory_{ device_ },
		textureSampler_{ [this](VkSampler sampler) { vkDestroySampler(device_, sampler, nullptr); } },
		vertexBuffer_{ device_ },
		vertexBufferMemory_{ device_ },
		indexBuffer_{ device_ },
		indexBufferMemory_{ device_ },
		uniformStagingBuffer_{ device_ },
		uniformStagingBufferMemory_{ device_ },
		uniformBuffer_{ device_ },
		uniformBufferMemory_{ device_ },
		descriptorPool_{ device_ },
		commandPool_{ device_ },
		imageAvaliableSemaphore_{ device_ },
		renderFinishedSemaphore_{ device_ }
		{}

	void VulkanApi::init(GLFWwindow *window) {
		glfwGetWindowSize(window, &windowWidth_, &windowHeight_);

		initInstance();
		initDebugReport();
		initSurface(window);
		initDevice();
		initSwapchain();
		initImageViews();
		initRenderPass();
		initDescriptorSetLayout();
		initGraphicsPipeline();
		initCommandPool();
		initDepthResource();
		initFramebuffers();
		initTextureImage();
		initTextureSampler();
		initVertexBuffer();
		initIndexBuffer();
		initUniformBuffer();
		initDescriptorPool();
		initDescriptorSet();
		initCommandBuffers();
		initSemaphores();
	}

	void VulkanApi::destroy() {
		vkDeviceWaitIdle(device_);
	}

	void VulkanApi::update() {
		updateUniformBuffer();
		drawFrame();
	}

	void VulkanApi::drawFrame() {
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device_, swapchain_, std::numeric_limits<uint64_t>::max(), imageAvaliableSemaphore_, VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchain();
			return;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			log::cout << "failed to get the next swapchain image" << std::endl;
			std::exit(-1);
		}


		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		std::array<VkSemaphore, 1> waitSemaphores{ imageAvaliableSemaphore_ };
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = &waitStage;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers_.at(imageIndex);

		std::array<VkSemaphore, 1> signalSemaphores{ renderFinishedSemaphore_ };
		submitInfo.signalSemaphoreCount = signalSemaphores.size();
		submitInfo.pSignalSemaphores = signalSemaphores.data();

		result = vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		if (result != VK_SUCCESS) {
			log::cout << "failed to do queue thing" << std::endl;
			std::exit(-1);
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = signalSemaphores.size();
		presentInfo.pWaitSemaphores = signalSemaphores.data();

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain_;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(device_.presentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain();
		} else if (result != VK_SUCCESS) {
			log::cout << "failed to present swapchain" << std::endl;
			std::exit(-1);
		}
	}

	void VulkanApi::onWindowResize(int width, int height) {
		windowWidth_ = width;
		windowHeight_ = height;
		recreateSwapchain();
	} 

	void VulkanApi::recreateSwapchain() {
		if (windowWidth_ == 0 || windowHeight_ == 0) { return; }
		vkDeviceWaitIdle(device_);

		initSwapchain();
		initImageViews();
		//initRenderPass();
		initGraphicsPipeline();
		initDepthResource();
		initFramebuffers();
		initCommandBuffers();
	}

	void VulkanApi::updateUniformBuffer() {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4{ 1.0f }, time * glm::radians(90.0f), glm::vec3{ 0.0f, 0.0f, 1.0f });
		ubo.view = glm::lookAt(glm::vec3{ 2.0f, 2.0f, 2.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f });
		ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(windowWidth_) / static_cast<float>(windowWidth_), 0.1f, 10.0f);

		ubo.proj[1][1] *= -1;
		//TODO: device whether or not to flip the y coord ubo.proj[1][1] *= -1;

		void *data;
		uniformStagingBufferMemory_.map(0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		uniformStagingBufferMemory_.unmap();

		copyBuffer(uniformStagingBuffer_, uniformBuffer_, sizeof(ubo));

	}

	void VulkanApi::initInstance() {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "hello app";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "oak engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		
		//instance setup
		VkInstanceCreateInfo instanceInfo{};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;

		//layers
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> layers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		log::cout << "avaliable layers: " << std::endl;

		for (const auto &it : layers) {
			log::cout << it.layerName << std::endl;
		}

		const char* layerName = "VK_LAYER_LUNARG_standard_validation";

		instanceInfo.enabledLayerCount = layerEnabledCount;
		instanceInfo.ppEnabledLayerNames = &layerName;

		//extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		log::cout << "glfw required extensions: " << std::endl;

		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			log::cout << glfwExtensions[i] << std::endl;
		}

		uint32_t extensionCount = 0;

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		log::cout << "avaliable extensions: " << std::endl;

		for (auto it : extensions) {
			log::cout << it.extensionName << std::endl;
		}

		extensionCount = glfwExtensionCount + 1;

		std::vector<const char*> extensionNames(extensionCount);
		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			extensionNames[i] = glfwExtensions[i];
		}

		extensionNames[glfwExtensionCount] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;


		instanceInfo.enabledExtensionCount = extensionCount;
		instanceInfo.ppEnabledExtensionNames = extensionNames.data();

		//create the instance
		VkResult result = vkCreateInstance(&instanceInfo, nullptr, instance_.replace());

		if (result != VK_SUCCESS) {
			log::cout << "failed to create vulkan instance" << std::endl;
			std::exit(-1);
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData) {

		log::cout << "validation layer: " << msg << std::endl;

		return VK_FALSE;
	}


	void VulkanApi::initDebugReport() {
		VkDebugReportCallbackCreateInfoEXT debugInfo{};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		debugInfo.pfnCallback = debugCallback;

		auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_, "vkCreateDebugReportCallbackEXT"));
		func(instance_, &debugInfo, nullptr, debugReport_.replace());
	}

	VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) {
		//if the format doesnt matter then choose the optimal format
		if (formats.size() == 1 && formats.at(0).format == VK_FORMAT_UNDEFINED) {
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
		}
		//find the next best format
		for (const auto &it : formats) {
			if (it.format == VK_FORMAT_B8G8R8A8_UNORM && it.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
				return it;
			}
		}

		return formats.at(0);
	}

	VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &modes, VkPresentModeKHR preferedMode) {
		for (const auto& it : modes) {
			if (it == preferedMode) { return it; }
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, int width, int height) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			VkExtent2D extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

			extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
			extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

			return extent;
		}
	}


	void VulkanApi::initSurface(GLFWwindow *window) {
		VkResult result = glfwCreateWindowSurface(instance_, window, nullptr, surface_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create the vulkan surface" << std::endl;
			std::exit(-1);
		}
	}

	void VulkanApi::initDevice() {
		device_.create(surface_);
	}

	void VulkanApi::initSwapchain() {
		const VDevice::SwapchainSupportDetails &details = device_.getSwapchainSupportDetails();		

		VkSurfaceFormatKHR format = chooseSurfaceFormat(details.formats);
		VkPresentModeKHR mode = choosePresentMode(details.modes, VK_PRESENT_MODE_MAILBOX_KHR);
		VkExtent2D extent = chooseSwapExtent(details.capabilities, windowWidth_, windowHeight_);

		//get image count for swap images
		uint32_t imageCount = details.capabilities.minImageCount + 1;
		if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
			imageCount = details.capabilities.maxImageCount;
		}

		//fill the (massive) create info struct
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface_;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = format.format;
		createInfo.imageColorSpace = format.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VDevice::QueueFamilyIndices indices = device_.getQueueFamilyIndices();
		std::array<uint32_t, 2> queueFamilyIndices { static_cast<uint32_t>(indices.graphicsFamily), static_cast<uint32_t>(indices.presentFamily) };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
			createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = details.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = mode;
		createInfo.clipped = VK_TRUE;
		//set the old swapchain 
		VkSwapchainKHR oldSwapchain = swapchain_;
		createInfo.oldSwapchain = oldSwapchain;

		VkSwapchainKHR newSwapchain;

		VkResult result = vkCreateSwapchainKHR(device_, &createInfo, nullptr, &newSwapchain);
		if (result != VK_SUCCESS) {
			log::cout << "failed to create swapchain" << std::endl;
		}

		swapchain_ = newSwapchain;

		//get the swapchain images
		vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, nullptr);
		swapchainImages_.resize(imageCount);
		vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, swapchainImages_.data());

		swapchainImageFormat_ = format.format;
		swapchainExtent_ = extent;
	}

	void VulkanApi::initImageViews() {
		swapchainImageViews_.resize(swapchainImages_.size(), VHandle<VkImageView>{ [this](VkImageView imageView){ vkDestroyImageView(device_, imageView, nullptr); } });

		for (uint32_t i = 0; i < swapchainImages_.size(); i++) {
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = swapchainImages_.at(i);
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = swapchainImageFormat_;
			viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VkResult result = vkCreateImageView(device_, &viewInfo, nullptr, swapchainImageViews_.at(i).replace());
			if (result != VK_SUCCESS) {
				log::cout << "failed to create image view" << std::endl;
				std::exit(-1);
			}
		}
	}

	void VulkanApi::initDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings {
			uboLayoutBinding,
			samplerLayoutBinding
		};
		
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindings.size();
		layoutInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(device_, &layoutInfo, nullptr, descriptorSetLayout_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create descriptor set layout" << std::endl;
			std::exit(-1);
		}



	}

	void VulkanApi::initGraphicsPipeline() {
		static bool inited = false;

		if (inited == false) {
			VShader vertModule{ &device_, VK_SHADER_STAGE_VERTEX_BIT };
			VShader fragModule{ &device_, VK_SHADER_STAGE_FRAGMENT_BIT };

			vertModule.load("src/graphics/shaders/basic/vert.spv");
			fragModule.load("src/graphics/shaders/basic/frag.spv");

			graphicsPipeline_.addBindingDescription({ 0, 0, sizeof(Vertex) });
			graphicsPipeline_.addAttributeDescription({ 
				{ 0, 0, ATTRIB_FORMAT_3F, offsetof(Vertex, Vertex::position) },
				{ 0, 1, ATTRIB_FORMAT_3F, offsetof(Vertex, Vertex::color) },
				{ 0, 2, ATTRIB_FORMAT_2F, offsetof(Vertex, Vertex::texCoords) }
			});

			graphicsPipeline_.addShaderModule(std::move(vertModule));
			graphicsPipeline_.addShaderModule(std::move(fragModule));
			inited = true;
		}

		graphicsPipeline_.create(descriptorSetLayout_, swapchainExtent_, renderPass_, 0);

	}

	void VulkanApi::initRenderPass() {
		renderPass_.create(swapchainImageFormat_, findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT));

	}

	void VulkanApi::initFramebuffers() {
		swapchainFramebuffers_.clear();
		swapchainFramebuffers_.resize(swapchainImageViews_.size(), { &device_ });

		for (size_t i = 0; i < swapchainImageViews_.size(); i++) {
			swapchainFramebuffers_.at(i).create({ swapchainImageViews_.at(i), depthImage_ }, renderPass_, swapchainExtent_.width, swapchainExtent_.height);
		}
	}

	void VulkanApi::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
		VkCommandBuffer commandBuffer;
		commandPool_.allocateBuffers(1, &commandBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device_.graphicsQueue());

		commandPool_.freeBuffers(1, &commandBuffer);
	}

	void VulkanApi::transitionImageLayout(VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout) {
		VkCommandBuffer commandBuffer;
		commandPool_.allocateBuffers(1, &commandBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectFlags;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		} else {
			log::cout << "invalid argument" << std::endl;
		}

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);


		vkEndCommandBuffer(commandBuffer);
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device_.graphicsQueue());

		commandPool_.freeBuffers(1, &commandBuffer);
	}

	void VulkanApi::copyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height) {
		VkCommandBuffer commandBuffer;
		commandPool_.allocateBuffers(1, &commandBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkImageSubresourceLayers subResource = {};
		subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subResource.baseArrayLayer = 0;
		subResource.mipLevel = 0;
		subResource.layerCount = 1;

		VkImageCopy region = {};
		region.srcSubresource = subResource;
		region.dstSubresource = subResource;
		region.srcOffset = { 0, 0, 0 };
		region.dstOffset = { 0, 0, 0 };
		region.extent.width = width;
		region.extent.height = height;
		region.extent.depth = 1;

		vkCmdCopyImage(
			commandBuffer, 
			srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
			dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &region);


		vkEndCommandBuffer(commandBuffer);
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device_.graphicsQueue());

		commandPool_.freeBuffers(1, &commandBuffer);
	}

	VkFormat VulkanApi::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(device_, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		log::cout << "failed to find supported features" << std::endl;
		std::exit(-1);
	}

	void VulkanApi::initDepthResource() {
		static bool inited = false;
		
		VkFormat depthFormat = findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		if (inited == true) {
			depthImage_.destroy();
			depthImageMemory_.free();
		}

		depthImage_.create(swapchainExtent_.width, swapchainExtent_.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);		
		depthImageMemory_.allocate(depthImage_.getMemoryRequirements(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		depthImageMemory_.bindImage(depthImage_, 0);
		depthImage_.createView(VK_IMAGE_ASPECT_DEPTH_BIT);

		inited = true;
		transitionImageLayout(depthImage_, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	void VulkanApi::initTextureImage() {

		int texWidth, texHeight, texChannels;
		stbi_uc *pixels = stbi_load("res/textures/tex.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			log::cout << "failed to load image" << std::endl;
			std::exit(-1);
		}

		VImage stagingImage{ device_ };
		VMemory stagingImageMemory{ device_ };

		stagingImage.create(texWidth, texHeight, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_LINEAR, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
		stagingImageMemory.allocate(stagingImage.getMemoryRequirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingImageMemory.bindImage(stagingImage, 0);


		void *data;
		stagingImageMemory.map(0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		stagingImageMemory.unmap();

		stbi_image_free(pixels);

		textureImage_.create(texWidth, texHeight, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
		textureImageMemory_.allocate(textureImage_.getMemoryRequirements(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		textureImageMemory_.bindImage(textureImage_, 0);
		
		transitionImageLayout(stagingImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		transitionImageLayout(textureImage_, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyImage(stagingImage, textureImage_, texWidth, texHeight);

		transitionImageLayout(textureImage_, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		textureImage_.createView(VK_IMAGE_ASPECT_COLOR_BIT);

	}

	void VulkanApi::initTextureSampler() {
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VkResult result = vkCreateSampler(device_, &samplerInfo, nullptr, textureSampler_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create the texture image sampler" << std::endl;
			std::exit(-1);
		}
	}

	void VulkanApi::initVertexBuffer() {
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
		
		//create a staging buffer
		VBuffer stagingBuffer{ device_ };
		VMemory stagingBufferMemory{ device_ };
		stagingBuffer.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		stagingBufferMemory.allocate(stagingBuffer.getMemoryRequirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBufferMemory.bindBuffer(stagingBuffer, 0);
		//copy vertices to the buffer
		void *data;
		stagingBufferMemory.map(0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), bufferSize);
		stagingBufferMemory.unmap();
		//create the vertex buffer
		vertexBuffer_.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		vertexBufferMemory_.allocate(vertexBuffer_.getMemoryRequirements(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vertexBufferMemory_.bindBuffer(vertexBuffer_, 0);
		//copy the buffer		
		copyBuffer(stagingBuffer, vertexBuffer_, bufferSize);
	}

	void VulkanApi::initIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
		
		//create a staging buffer
		VBuffer stagingBuffer{ device_ };
		VMemory stagingBufferMemory{ device_ };
		stagingBuffer.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		stagingBufferMemory.allocate(stagingBuffer.getMemoryRequirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBufferMemory.bindBuffer(stagingBuffer, 0);
		//copy vertices to the buffer
		void *data;
		stagingBufferMemory.map(0, bufferSize, 0, &data);
		memcpy(data, indices.data(), bufferSize);
		stagingBufferMemory.unmap();
		//create the index buffer
		indexBuffer_.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		indexBufferMemory_.allocate(indexBuffer_.getMemoryRequirements(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		indexBufferMemory_.bindBuffer(indexBuffer_, 0);
		//copy the buffer
		copyBuffer(stagingBuffer, indexBuffer_, bufferSize);
	}

	void VulkanApi::initUniformBuffer() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		//create the staging buffer
		uniformStagingBuffer_.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		uniformStagingBufferMemory_.allocate(uniformStagingBuffer_.getMemoryRequirements(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		uniformStagingBufferMemory_.bindBuffer(uniformStagingBuffer_, 0);

		//create the uniform buffer
		uniformBuffer_.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		uniformBufferMemory_.allocate(uniformBuffer_.getMemoryRequirements(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		uniformBufferMemory_.bindBuffer(uniformBuffer_, 0);
	}

	void VulkanApi::initDescriptorPool() {
		descriptorPool_.create({ { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 } });
	}

	void VulkanApi::initDescriptorSet() {
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool_;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout_;

		VkResult result = vkAllocateDescriptorSets(device_, &allocInfo, &descriptorSet_);
		if (result != VK_SUCCESS) {
			log::cout << "failed to allocate the descriptor set" << std::endl;
			std::exit(-1);
		}

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffer_;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImage_;
		imageInfo.sampler = textureSampler_;


		VkWriteDescriptorSet descriptorWrites[2];
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSet_;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		descriptorWrites[0].pImageInfo = nullptr;
		descriptorWrites[0].pTexelBufferView = nullptr;
		descriptorWrites[0].pNext = nullptr;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSet_;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = nullptr;
		descriptorWrites[1].pImageInfo = &imageInfo;
		descriptorWrites[1].pTexelBufferView = nullptr;
		descriptorWrites[1].pNext = nullptr;

		vkUpdateDescriptorSets(device_, 2, descriptorWrites, 0, nullptr);
	}

	void VulkanApi::initCommandPool() {
		VDevice::QueueFamilyIndices indices = device_.getQueueFamilyIndices();
		commandPool_.create(indices.graphicsFamily);
	}

	void VulkanApi::initCommandBuffers() {

		if (commandBuffers_.size() > 0) {
			commandPool_.freeBuffers(commandBuffers_.size(), commandBuffers_.data());
		}

		commandBuffers_.resize(swapchainFramebuffers_.size());
		commandPool_.allocateBuffers(commandBuffers_.size(), commandBuffers_.data(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		for (size_t i = 0; i < commandBuffers_.size(); i++) {
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = nullptr;

			vkBeginCommandBuffer(commandBuffers_.at(i), &beginInfo);

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass_;
			renderPassInfo.framebuffer = swapchainFramebuffers_.at(i);
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapchainExtent_;

			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = { 0.0f, 0.2f, 0.65f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = clearValues.size();
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers_.at(i), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(commandBuffers_.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_);

			vkCmdBindDescriptorSets(commandBuffers_.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_.getLayout(), 0, 1, &descriptorSet_, 0, nullptr);

			//dynamic state
			VkViewport vp = {0, 0, static_cast<float>(windowWidth_), static_cast<float>(windowHeight_), 0.0f, 1.0f};

			vkCmdSetViewport(commandBuffers_.at(i), 0, 1, &vp);
			vkCmdSetLineWidth(commandBuffers_.at(i), 1.0f);

			//draw
			VkBuffer vertexBuffer = vertexBuffer_;
			VkDeviceSize ofst = 0;
			vkCmdBindVertexBuffers(commandBuffers_.at(i), 0, 1, &vertexBuffer, &ofst);

			vkCmdBindIndexBuffer(commandBuffers_.at(i), indexBuffer_, 0, VK_INDEX_TYPE_UINT16);

			vkCmdDrawIndexed(commandBuffers_.at(i), indices.size(), 1, 0, 0, 0);
			
			vkCmdEndRenderPass(commandBuffers_.at(i));

			VkResult result = vkEndCommandBuffer(commandBuffers_.at(i));
			if (result != VK_SUCCESS) {
				log::cout << "failed to record the command buffer" << std::endl;
				std::exit(-1);
			}
		}
	}

	void VulkanApi::initSemaphores() {
		imageAvaliableSemaphore_.create();
		renderFinishedSemaphore_.create();
	}

}