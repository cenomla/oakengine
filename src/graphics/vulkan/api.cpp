#include "api.h"

#include <limits>
#include <set>
#include <GLFW/glfw3.h>

#include "log.h"

namespace oak::graphics {

	VulkanApi::VulkanApi() : instance_{ [](VkInstance instance) { vkDestroyInstance(instance, nullptr); } }, 
		debugReport_{ [this](VkDebugReportCallbackEXT callback) { 
			auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_, "vkDestroyDebugReportCallbackEXT"));
			func(this->instance_, callback, nullptr);
		} },
		surface_{ [this](VkSurfaceKHR surface) { vkDestroySurfaceKHR(this->instance_, surface, nullptr); } },
		device_{ [](VkDevice device) { vkDestroyDevice(device, nullptr); } },
		swapchain_{ [this](VkSwapchainKHR swapchain) { vkDestroySwapchainKHR(this->device_, swapchain, nullptr); } },
		renderPass_{ [this](VkRenderPass pass) { vkDestroyRenderPass(this->device_, pass, nullptr); } },
		pipelineLayout_{ [this](VkPipelineLayout layout) { vkDestroyPipelineLayout(this->device_, layout, nullptr); } },
		graphicsPipeline_{ [this](VkPipeline pipeline) { vkDestroyPipeline(this->device_, pipeline, nullptr); } },
		commandPool_{ [this](VkCommandPool pool) { vkDestroyCommandPool(this->device_, pool, nullptr); } },
		imageAvaliableSemaphore_{ [this](VkSemaphore semaphore) { vkDestroySemaphore(this->device_, semaphore, nullptr); } },
		renderFinishedSemaphore_{ [this](VkSemaphore semaphore) { vkDestroySemaphore(this->device_, semaphore, nullptr); } }
		{}

	void VulkanApi::init(GLFWwindow *window) {
		initInstance();
		initDebugReport();
		initSurface(window);
		initDevice();
		initSwapchain();
		initImageViews();
		initRenderPass();
		initGraphicsPipeline();
		initFramebuffers();
		initCommandPool();
		initCommandBuffers();
		initSemaphores();
	}

	void VulkanApi::destroy() {
		vkDeviceWaitIdle(device_);
	}

	void VulkanApi::update() {
		uint32_t imageIndex;
		vkAcquireNextImageKHR(device_, swapchain_, std::numeric_limits<uint64_t>::max(), imageAvaliableSemaphore_, VK_NULL_HANDLE, &imageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphore[] = { imageAvaliableSemaphore_ };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers_.at(imageIndex);

		VkSemaphore signalSemaphore[] = { renderFinishedSemaphore_ };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphore;

		VkResult result = vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
		if (result != VK_SUCCESS) {
			log::cout << "failed to do queue thing" << std::endl;
			std::exit(-1);
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphore;

		VkSwapchainKHR swapchains[] = { swapchain_ };

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(presentQueue_, &presentInfo);
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

		const char* layerNames[] = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		instanceInfo.enabledLayerCount = 1;
		instanceInfo.ppEnabledLayerNames = layerNames;

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
			log::cout << "couldnt create vulkan instance" << std::endl;
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


	VulkanApi::QueueFamilyIndices VulkanApi::findQueueFamilyIndices(VkPhysicalDevice device) {
		QueueFamilyIndices indices;
		
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		log::cout << "queue family count: " << queueFamilyCount << std::endl;

		int i = 0;
		for (const auto &queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.complete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	VulkanApi::SwapchainSupportDetails VulkanApi::getSwapchainSupport(VkPhysicalDevice device) {
		SwapchainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
		}

		uint32_t modesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &modesCount, nullptr);
		if (modesCount != 0) {
			details.modes.resize(modesCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &modesCount, details.modes.data());	
		}

		return details;
	}

	VkPhysicalDevice VulkanApi::getPhysicalDevice() {
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		//enumerate physical devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

		//drive amount of devies
		log::cout << "device count: " << deviceCount << std::endl;

		//check for a suitable device
		for (const auto &device : devices) {
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


			physicalDevice = device;
		}

		return physicalDevice;
	}

	VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) {
		//if the format doesnt matter then choose the optimal format
		if (formats.size() == 1 && formats.at(0).format == VK_FORMAT_UNDEFINED) {
			return { VK_FORMAT_B8G8R8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		//find the next best format
		for (const auto &it : formats) {
			if (it.format == VK_FORMAT_B8G8R8_UNORM && it.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return it;
			}
		}

		return formats.at(0);
	}

	VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &modes) {
		for (const auto& it : modes) {
			if (it == VK_PRESENT_MODE_MAILBOX_KHR) { return it; }
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			VkExtent2D extent = {1280, 720};

			extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
			extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

			return extent;
		}
	}

	void VulkanApi::initDevice() {
		//pick a physical device
		physicalDevice_ = getPhysicalDevice();

		//check if the device meets requirements
		QueueFamilyIndices queueFamilyIndices = findQueueFamilyIndices(physicalDevice_);
		SwapchainSupportDetails swapChainSupportDetails = getSwapchainSupport(physicalDevice_);

		if (!queueFamilyIndices.complete() || swapChainSupportDetails.formats.empty() || swapChainSupportDetails.modes.empty()) {
			log::cout << "no supported devices found" << std::endl;
			std::exit(-1);
		}

		//create queue family create infos
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<int> uniqueQueueFamilies{ queueFamilyIndices.graphicsFamily, queueFamilyIndices.presentFamily };

		float queuePriority = 1.0f;

		for (int queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = queueFamily;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueInfo);
		}
		
		//create a logical device
		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo deviceInfo{};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceInfo.pEnabledFeatures = &deviceFeatures;


		//setup device extensions and layers
		const char* layerNames[] = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		const char* deviceExtensions[] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		deviceInfo.enabledLayerCount = 1;
		deviceInfo.ppEnabledLayerNames = layerNames;
		deviceInfo.enabledExtensionCount = 1;
		deviceInfo.ppEnabledExtensionNames = deviceExtensions;

		//create the device and make sure it was successful
		VkResult result = vkCreateDevice(physicalDevice_, &deviceInfo, nullptr, device_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "couldnt create device" << std::endl;
			std::exit(-1);
		}
		//get the queues from the device;
		vkGetDeviceQueue(device_, queueFamilyIndices.graphicsFamily, 0, &graphicsQueue_);
		vkGetDeviceQueue(device_, queueFamilyIndices.presentFamily, 0, &presentQueue_);
	}

	void VulkanApi::initSurface(GLFWwindow *window) {
		VkResult result = glfwCreateWindowSurface(instance_, window, nullptr, surface_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create the vulkan surface" << std::endl;
			std::exit(-1);
		}
	}

	void VulkanApi::initSwapchain() {
		SwapchainSupportDetails details = getSwapchainSupport(physicalDevice_);		

		VkSurfaceFormatKHR format = chooseSurfaceFormat(details.formats);
		VkPresentModeKHR mode = choosePresentMode(details.modes);
		VkExtent2D extent = chooseSwapExtent(details.capabilities);

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

		QueueFamilyIndices indices = findQueueFamilyIndices(physicalDevice_);
		uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(indices.graphicsFamily), static_cast<uint32_t>(indices.presentFamily) };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = details.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = mode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(device_, &createInfo, nullptr, swapchain_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create swapchain" << std::endl;
		}

		//get the swapchain images
		vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, nullptr);
		swapchainImages_.resize(imageCount);
		vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, swapchainImages_.data());

		swapchainImageFormat_ = format.format;
		swapchainExtent_ = extent;
	}

	void VulkanApi::initImageViews() {
		swapchainImageViews_.resize(swapchainImages_.size(), VHandle<VkImageView>{ [this](VkImageView imageView){ vkDestroyImageView(this->device_, imageView, nullptr); } });

		for (uint32_t i = 0; i < swapchainImages_.size(); i++) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapchainImages_.at(i);
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapchainImageFormat_;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkResult result = vkCreateImageView(device_, &createInfo, nullptr, swapchainImageViews_.at(i).replace());
			if (result != VK_SUCCESS) {
				log::cout << "failed to create image views" << std::endl;
				std::exit(-1);
			}
		}
	}

	static std::vector<char> readFile(const std::string &path) {
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			log::cout << "couldnt open file: " << path << std::endl;
			std::exit(-1);
		}

		size_t size = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(size);
		file.seekg(0);
		file.read(buffer.data(), size);

		file.close();

		return buffer;
	}

	void VulkanApi::createShaderModule(const std::vector<char> &code, VHandle<VkShaderModule>& module) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkResult result = vkCreateShaderModule(device_, &createInfo, nullptr, module.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create shader module" << std::endl;
			std::exit(-1);
		}
	}

	void VulkanApi::initGraphicsPipeline() {
		auto vertShaderCode = readFile("src/graphics/shaders/basic/vert.spv");
		auto fragShaderCode = readFile("src/graphics/shaders/basic/frag.spv");

		VHandle<VkShaderModule> vertModule{ [this](VkShaderModule module){ vkDestroyShaderModule(this->device_, module, nullptr); } };
		VHandle<VkShaderModule> fragModule{ [this](VkShaderModule module){ vkDestroyShaderModule(this->device_, module, nullptr); } };

		createShaderModule(vertShaderCode, vertModule);
		createShaderModule(fragShaderCode, fragModule);

		VkPipelineShaderStageCreateInfo vertStageInfo = {};
		vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertStageInfo.module = vertModule;
		vertStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragStageInfo = {};
		fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragStageInfo.module = fragModule;
		fragStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageInfo, fragStageInfo };

		VkPipelineVertexInputStateCreateInfo vertInputInfo = {};
		vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertInputInfo.vertexBindingDescriptionCount = 0;
		vertInputInfo.pVertexBindingDescriptions = nullptr;
		vertInputInfo.vertexAttributeDescriptionCount = 0;
		vertInputInfo.pVertexAttributeDescriptions = nullptr;

		//fixed function

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchainExtent_.width);
		viewport.height = static_cast<float>(swapchainExtent_.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = {0, 0};
		scissor.extent = swapchainExtent_;

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
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling= {};
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

		//setup the pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		VkResult result = vkCreatePipelineLayout(device_, &pipelineLayoutCreateInfo, nullptr, pipelineLayout_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "couldnt create a pipeline layout" << std::endl;
			std::exit(-1);
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;
		pipelineInfo.layout = pipelineLayout_;
		pipelineInfo.renderPass = renderPass_;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		result = vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, graphicsPipeline_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "couldnt create the graphics pipeline" << std::endl;
			std::exit(-1);
		}

	}

	void VulkanApi::initRenderPass() {
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = swapchainImageFormat_;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subPass = {};
		subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subPass.colorAttachmentCount = 1;
		subPass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subPass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult result = vkCreateRenderPass(device_, &renderPassInfo, nullptr, renderPass_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "couldnt create render pass" << std::endl;
			std::exit(-1);
		}


	}

	void VulkanApi::initFramebuffers() {
		swapchainFramebuffers_.resize(swapchainImageViews_.size(), VHandle<VkFramebuffer>{ 
			[this](VkFramebuffer framebuffer) { vkDestroyFramebuffer(this->device_, framebuffer, nullptr); } });

		for (size_t i = 0; i < swapchainImageViews_.size(); i++) {
			VkImageView attachments[] = {
				swapchainImageViews_[i]
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass_;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapchainExtent_.width;
			framebufferInfo.height = swapchainExtent_.height;
			framebufferInfo.layers = 1;

			VkResult result = vkCreateFramebuffer(device_, &framebufferInfo, nullptr, swapchainFramebuffers_.at(i).replace());
			if (result != VK_SUCCESS) {
				log::cout << "counldnt create a framebuffer" << std::endl;
				std::exit(-1);
			}

		}
	}

	void VulkanApi::initCommandPool() {
		QueueFamilyIndices indices = findQueueFamilyIndices(physicalDevice_);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = indices.graphicsFamily;
		poolInfo.flags = 0;


		VkResult result = vkCreateCommandPool(this->device_, &poolInfo, nullptr, commandPool_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "couldnt create commnad pool" << std::endl;
			std::exit(-1);
		}
	}

	void VulkanApi::initCommandBuffers() {
		commandBuffers_.resize(swapchainFramebuffers_.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool_;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());

		VkResult result = vkAllocateCommandBuffers(device_, &allocInfo, commandBuffers_.data());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create command buffers" << std::endl;
			std::exit(-1);
		}

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

			VkClearValue clearColor = { 0.3f, 0.2f, 0.7f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(commandBuffers_.at(i), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(commandBuffers_.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_);
			vkCmdDraw(commandBuffers_.at(i), 3, 1, 0, 0);
			vkCmdEndRenderPass(commandBuffers_.at(i));

			VkResult result = vkEndCommandBuffer(commandBuffers_.at(i));
			if (result != VK_SUCCESS) {
				log::cout << "failed to record the command buffer" << std::endl;
				std::exit(-1);
			}
		}
	}

	void VulkanApi::initSemaphores() {
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, imageAvaliableSemaphore_.replace()) != VK_SUCCESS ||
			vkCreateSemaphore(device_, &semaphoreInfo, nullptr, renderFinishedSemaphore_.replace()) != VK_SUCCESS ) {
			log::cout << "failed to create semaphores" << std::endl;
			std::exit(-1);
		}

	}

}