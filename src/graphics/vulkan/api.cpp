#include "api.h"

#include <cstring>
#include <limits>
#include <chrono>
#include <set>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_USE_PNG_ONLY
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
		device_{ [](VkDevice device) { vkDestroyDevice(device, nullptr); } },
		swapchain_{ [this](VkSwapchainKHR swapchain) { vkDestroySwapchainKHR(device_, swapchain, nullptr); } },
		renderPass_{ [this](VkRenderPass pass) { vkDestroyRenderPass(device_, pass, nullptr); } },
		descriptorSetLayout_{ [this](VkDescriptorSetLayout layout) { vkDestroyDescriptorSetLayout(device_, layout, nullptr); } },
		graphicsPipeline_{ &device_ },
		depthImage_{ [this](VkImage image) { vkDestroyImage(device_, image, nullptr); } },
		depthImageMemory_{ [this](VkDeviceMemory memory) { vkFreeMemory(device_, memory, nullptr); } },
		depthImageView_{ [this](VkImageView view) { vkDestroyImageView(device_, view, nullptr); } },
		textureImage_{ [this](VkImage image) { vkDestroyImage(device_, image, nullptr); } },
		textureImageMemory_{ [this](VkDeviceMemory memory) { vkFreeMemory(device_, memory, nullptr); } },
		textureImageView_{ [this](VkImageView view) { vkDestroyImageView(device_, view, nullptr); } },
		textureSampler_{ [this](VkSampler sampler) { vkDestroySampler(device_, sampler, nullptr); } },
		vertexBuffer_{ [this](VkBuffer buffer) { vkDestroyBuffer(device_, buffer, nullptr); } },
		vertexBufferMemory_{ [this](VkDeviceMemory mem) { vkFreeMemory(device_, mem, nullptr); } },
		indexBuffer_{ [this](VkBuffer buffer) { vkDestroyBuffer(device_, buffer, nullptr); } },
		indexBufferMemory_{ [this](VkDeviceMemory mem) { vkFreeMemory(device_, mem, nullptr); } },
		uniformStagingBuffer_{ [this](VkBuffer buffer) { vkDestroyBuffer(device_, buffer, nullptr); } },
		uniformStagingBufferMemory_{ [this](VkDeviceMemory mem) { vkFreeMemory(device_, mem, nullptr); } },
		uniformBuffer_{ [this](VkBuffer buffer) { vkDestroyBuffer(device_, buffer, nullptr); } },
		uniformBufferMemory_{ [this](VkDeviceMemory mem) { vkFreeMemory(device_, mem, nullptr); } },
		descriptorPool_{ [this](VkDescriptorPool pool) { vkDestroyDescriptorPool(device_, pool, nullptr); } },
		commandPool_{ [this](VkCommandPool pool) { vkDestroyCommandPool(device_, pool, nullptr); } },
		imageAvaliableSemaphore_{ [this](VkSemaphore semaphore) { vkDestroySemaphore(device_, semaphore, nullptr); } },
		renderFinishedSemaphore_{ [this](VkSemaphore semaphore) { vkDestroySemaphore(device_, semaphore, nullptr); } }
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
		initTextureImageView();
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

		result = vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
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

		result = vkQueuePresentKHR(presentQueue_, &presentInfo);
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
		initRenderPass();
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
		vkMapMemory(device_, uniformStagingBufferMemory_, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device_, uniformStagingBufferMemory_);

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

		const char* layerNames[] = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		instanceInfo.enabledLayerCount = layerEnabledCount;
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


			log::cout << "device name: " << deviceProperties.deviceName << std::endl;
			//physicalDevice = device;
		}

		physicalDevice = devices[0];

		return physicalDevice;
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

		deviceInfo.enabledLayerCount = layerEnabledCount;
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
			createImageView(swapchainImages_.at(i), swapchainImageFormat_, VK_IMAGE_ASPECT_COLOR_BIT, swapchainImageViews_.at(i));
		}
	} 

	VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Vertex::position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Vertex::color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, Vertex::texCoords);

		return attributeDescriptions;
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

		VkDescriptorSetLayoutBinding bindings[] = {
			uboLayoutBinding,
			samplerLayoutBinding
		};
		
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 2;
		layoutInfo.pBindings = bindings;

		VkResult result = vkCreateDescriptorSetLayout(device_, &layoutInfo, nullptr, descriptorSetLayout_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create descriptor set layout" << std::endl;
			std::exit(-1);
		}



	}

	void VulkanApi::initGraphicsPipeline() {
		static bool inited = false;


		if (inited == false) {
			VShader vertModule{ device_, VK_SHADER_STAGE_VERTEX_BIT };
			VShader fragModule{ device_, VK_SHADER_STAGE_FRAGMENT_BIT };

			vertModule.load("src/graphics/shaders/basic/vert.spv");
			fragModule.load("src/graphics/shaders/basic/frag.spv");

			auto bindingDescription = getBindingDescription();
			auto attributeDescriptions = getAttributeDescriptions();

			graphicsPipeline_.addBindingDescription(bindingDescription);
			for (auto attr : attributeDescriptions) {
				graphicsPipeline_.addAttributeDescription(attr);
			}

			graphicsPipeline_.addShaderModule(std::move(vertModule));
			graphicsPipeline_.addShaderModule(std::move(fragModule));
			inited = true;
		}

		graphicsPipeline_.create(descriptorSetLayout_, swapchainExtent_, renderPass_, 0);

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

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subPass = {};
		subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subPass.colorAttachmentCount = 1;
		subPass.pColorAttachments = &colorAttachmentRef;
		subPass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 2;
		renderPassInfo.pAttachments = attachments;
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
			[this](VkFramebuffer framebuffer) { vkDestroyFramebuffer(device_, framebuffer, nullptr); } });

		for (size_t i = 0; i < swapchainImageViews_.size(); i++) {
			VkImageView attachments[] = {
				swapchainImageViews_[i],
				depthImageView_
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass_;
			framebufferInfo.attachmentCount = 2;
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

	uint32_t VulkanApi::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		
		log::cout << "failed to find suitable memory type" << std::endl;
		std::exit(-1);
	}

	void VulkanApi::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VHandle<VkImage> &image, VHandle<VkDeviceMemory> &memory) {
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		VkResult result = vkCreateImage(device_, &imageInfo, nullptr, image.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create image" << std::endl;
			std::exit(-1);
		}

		VkMemoryRequirements memRequirements = {};
		vkGetImageMemoryRequirements(device_, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		result = vkAllocateMemory(device_, &allocInfo, nullptr, memory.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to allocate image memory" << std::endl;
			std::exit(-1);
		}

		vkBindImageMemory(device_, image, memory, 0);
	}

	void VulkanApi::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VHandle<VkBuffer> &buffer, VHandle<VkDeviceMemory> &memory) {
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateBuffer(device_, &bufferInfo, nullptr, buffer.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create vertex buffer" << std::endl;
			std::exit(-1);
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		result = vkAllocateMemory(device_, &allocInfo, nullptr, memory.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to allocate memory for the vertex buffer" << std::endl;
			std::exit(-1);
		}

		vkBindBufferMemory(device_, buffer, memory, 0);
	}

	void VulkanApi::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool_;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

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

		vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue_);

		vkFreeCommandBuffers(device_, commandPool_, 1, &commandBuffer);
	}

	void VulkanApi::transitionImageLayout(VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout) {
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool_;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

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

		vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue_);

		vkFreeCommandBuffers(device_, commandPool_, 1, &commandBuffer);
	}

	void VulkanApi::copyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height) {
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool_;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

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

		vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue_);

		vkFreeCommandBuffers(device_, commandPool_, 1, &commandBuffer);		
	}

	void VulkanApi::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VHandle<VkImageView> &imageView) {
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(device_, &viewInfo, nullptr, imageView.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create image view" << std::endl;
			std::exit(-1);
		}
	}

	VkFormat VulkanApi::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice_, format, &props);

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
		VkFormat depthFormat = findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		createImage(swapchainExtent_.width, swapchainExtent_.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage_, depthImageMemory_);
		createImageView(depthImage_, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, depthImageView_);

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

		VHandle<VkImage> stagingImage{ [this](VkImage image) { vkDestroyImage(device_, image, nullptr); } };
		VHandle<VkDeviceMemory> stagingImageMemory{ [this](VkDeviceMemory memory) { vkFreeMemory(device_, memory, nullptr); } };

		createImage(
			texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_LINEAR, 
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			stagingImage, stagingImageMemory);
		


		void *data;
		vkMapMemory(device_, stagingImageMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device_, stagingImageMemory);

		stbi_image_free(pixels);

		createImage(
			texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			textureImage_, textureImageMemory_);
		
		transitionImageLayout(stagingImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		transitionImageLayout(textureImage_, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyImage(stagingImage, textureImage_, texWidth, texHeight);

		transitionImageLayout(textureImage_, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


	}

	void VulkanApi::initTextureImageView() {
		createImageView(textureImage_, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, textureImageView_);
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
		VHandle<VkBuffer> stagingBuffer{ [this](VkBuffer buffer){ vkDestroyBuffer(device_, buffer, nullptr); } };
		VHandle<VkDeviceMemory> stagingBufferMemory{ [this](VkDeviceMemory memory){ vkFreeMemory(device_, memory, nullptr); } };
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		//copy vertices to the buffer
		void *data;
		vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), bufferSize);
		vkUnmapMemory(device_, stagingBufferMemory);
		//create the actual buffer and copy the info over
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer_, vertexBufferMemory_);		
		copyBuffer(stagingBuffer, vertexBuffer_, bufferSize);
	}

	void VulkanApi::initIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
		
		//create a staging buffer
		VHandle<VkBuffer> stagingBuffer{ [this](VkBuffer buffer){ vkDestroyBuffer(device_, buffer, nullptr); } };
		VHandle<VkDeviceMemory> stagingBufferMemory{ [this](VkDeviceMemory memory){ vkFreeMemory(device_, memory, nullptr); } };
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		//copy vertices to the buffer
		void *data;
		vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), bufferSize);
		vkUnmapMemory(device_, stagingBufferMemory);
		//create the actual buffer and copy the info over
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer_, indexBufferMemory_);		
		copyBuffer(stagingBuffer, indexBuffer_, bufferSize);
	}

	void VulkanApi::initUniformBuffer() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformStagingBuffer_, uniformStagingBufferMemory_);
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, uniformBuffer_, uniformBufferMemory_);

	}

	void VulkanApi::initDescriptorPool() {
		VkDescriptorPoolSize poolSizes[] = {
			{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1},
			{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = 1}
		};

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 2;
		poolInfo.pPoolSizes = poolSizes;
		poolInfo.maxSets = 1;

		VkResult result = vkCreateDescriptorPool(device_, &poolInfo, nullptr, descriptorPool_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "failed to create descriptor pool" << std::endl;
			std::exit(-1);
		}
	}

	void VulkanApi::initDescriptorSet() {
		VkDescriptorSetLayout layouts[] = { descriptorSetLayout_ };
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool_;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts;

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
		imageInfo.imageView = textureImageView_;
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

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSet_;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = nullptr;
		descriptorWrites[1].pImageInfo = &imageInfo;
		descriptorWrites[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(device_, 2, descriptorWrites, 0, nullptr);
	}

	void VulkanApi::initCommandPool() {
		QueueFamilyIndices indices = findQueueFamilyIndices(physicalDevice_);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = indices.graphicsFamily;
		poolInfo.flags = 0;


		VkResult result = vkCreateCommandPool(device_, &poolInfo, nullptr, commandPool_.replace());
		if (result != VK_SUCCESS) {
			log::cout << "couldnt create commnad pool" << std::endl;
			std::exit(-1);
		}
	}

	void VulkanApi::initCommandBuffers() {

		if (commandBuffers_.size() > 0) {
			vkFreeCommandBuffers(device_, commandPool_, commandBuffers_.size(), commandBuffers_.data());
		}

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
			VkBuffer vertexBuffers[] = { vertexBuffer_ };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers_.at(i), 0, 1, vertexBuffers, offsets);

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
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, imageAvaliableSemaphore_.replace()) != VK_SUCCESS ||
			vkCreateSemaphore(device_, &semaphoreInfo, nullptr, renderFinishedSemaphore_.replace()) != VK_SUCCESS ) {
			log::cout << "failed to create semaphores" << std::endl;
			std::exit(-1);
		}

	}

}