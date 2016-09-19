#include "api.h"

#include <vector>
#include <set>
#include <GLFW/glfw3.h>

#include "log.h"

namespace oak::graphics {

	void VulkanApi::init(GLFWwindow *window) {
		initInstance();
		initDebugReport();
		initSurface(window);
		initDevice();
	}

	void VulkanApi::destroy() {


		vkDestroyDevice(device_, nullptr);
		vkDestroySurfaceKHR(instance_, surface_, nullptr);

		auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_, "vkDestroyDebugReportCallbackEXT"));
		func(instance_, debugReport_, nullptr);

		vkDestroyInstance(instance_, nullptr);
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
		VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance_);

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
		func(instance_, &debugInfo, nullptr, &debugReport_);
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

	void VulkanApi::initDevice() {
		//pick a physical device
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

		log::cout << "device count: " << deviceCount << std::endl;

		for (const auto &device : devices) {
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			log::cout << deviceProperties.deviceName << std::endl;
			log::cout << deviceProperties.deviceType << std::endl;

			physicalDevice = device;
		}

		physicalDevice = devices[0];

		//create queue family create infos
		QueueFamilyIndices queueFamilyIndices = findQueueFamilyIndices(physicalDevice);

		if (!queueFamilyIndices.complete()) {
			log::cout 
				<< "device does not support queue: " 
				<< (queueFamilyIndices.graphicsFamily >= 0 ? "graphics" : "") 
				<< (queueFamilyIndices.presentFamily >= 0 ? ", present" : "") 
				<< std::endl;
			std::exit(-1);
		}

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

		deviceInfo.enabledExtensionCount = 0;
		deviceInfo.enabledLayerCount = 1;
		deviceInfo.ppEnabledLayerNames = layerNames;

		//create the device and make sure it was successful
		VkResult result = vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device_);
		if (result != VK_SUCCESS) {
			log::cout << "couldnt create device" << std::endl;
			std::exit(-1);
		}
		//get the queues from the device;
		vkGetDeviceQueue(device_, queueFamilyIndices.graphicsFamily, 0, &graphicQueue_);
		vkGetDeviceQueue(device_, queueFamilyIndices.presentFamily, 0, &presentQueue_);
	}

	void VulkanApi::initSurface(GLFWwindow *window) {
		VkResult result = glfwCreateWindowSurface(instance_, window, nullptr, &surface_);
		if (result != VK_SUCCESS) {
			log::cout << "failed to create the vulkan surface" << std::endl;
			std::exit(-1);
		}
	}

}