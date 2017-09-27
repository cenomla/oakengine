#include "vk_api.h"

#include <GLFW/glfw3.h>
#include <set>

#include <event_manager.h>
#include <input_events.h>
#include <log.h>

namespace oak::graphics {

	void VKApi::init() {
		if (!glfwVulkanSupported()) {
			log_print_err("vulkan is not supported");
			abort();
		}
		initInstance();
		initDebugReport();

		//create window
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window_ = glfwCreateWindow(1280, 720, "sandbox", nullptr, nullptr);
		if (!window_) {
			log_print_err("failed to create window");
			abort();
		}

		getEventQueue<WindowCreateEvent>().emit({ window_ });

		//init surface
		VkResult result = glfwCreateWindowSurface(instance_, window_, nullptr, &surface_);
		if (result != VK_SUCCESS) {
			log_print_err("failed to create vulkan surface"); abort();
		}

		initDevice();


	}

	void VKApi::terminate() {
		vkDestroyInstance(instance_, nullptr);
	}

	void VKApi::swap() {
		
	}

	void VKApi::initInstance() {
		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "sandbox";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.pEngineName = "oak engine";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instanceInfo;
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;

		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		oak::vector<VkLayerProperties> layers{ layerCount };
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		log_print_out("layers: ");
		for (const auto& layer : layers) {
			log_print_out("%s", layer.layerName);
		}

		const char* layerName = "VK_LAYER_LUNARG_standard_validation";

		instanceInfo.enabledLayerCount = 1;
		instanceInfo.ppEnabledLayerNames = &layerName;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		uint32_t extensionCount = 0;

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		oak::vector<VkExtensionProperties> extensions{ extensionCount };
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		extensionCount = glfwExtensionCount + 1;

		log_print_out("extensions: ");
		for (const auto& ext : extensions) {
			log_print_out("%s", ext.extensionName);
		}

		oak::vector<const char*> extNames{ extensionCount };
		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			extNames[i] = glfwExtensions[i];
		}
		extNames[glfwExtensionCount] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;

		instanceInfo.enabledExtensionCount = extensionCount;
		instanceInfo.ppEnabledExtensionNames = extNames.data();
		instanceInfo.flags = 0;
		instanceInfo.pNext = nullptr;

		VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance_);
		if (result != VK_SUCCESS) {
			log_print_err("vulkan instance creation failed");
			abort();
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

		log_print_warn("validation layer: %s", msg);

		return VK_FALSE;
	}


	void VKApi::initDebugReport() {
		VkDebugReportCallbackCreateInfoEXT debugInfo{};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		debugInfo.pfnCallback = debugCallback;

		auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_, "vkCreateDebugReportCallbackEXT"));
		func(instance_, &debugInfo, nullptr, &debugReport_);
	}

	void VKApi::getPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
		oak::vector<VkPhysicalDevice> physicalDevices{ deviceCount };
		vkEnumeratePhysicalDevices(instance_, &deviceCount, physicalDevices.data());

		log_print_out("device count: %u", deviceCount);

		for (const auto& device : physicalDevices) {
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			log_print_out("device name: %s", deviceProperties.deviceName);
		}

		physicalDevice_ = physicalDevices[0];
	}

	void VKApi::getFamilyIndices() {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);
		oak::vector<VkQueueFamilyProperties> queueFamilies{ queueFamilyCount };
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto &queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				deviceGraphicsQueue_ = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice_, i, surface_, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport) {
				devicePresentQueue_ = i;
			}

			if (deviceGraphicsQueue_ != 0 && devicePresentQueue_ != 0) {
				break;
			}

			i++;
		}
	}

	void VKApi::getDeviceSwapchainSupport() {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice_, surface_, &surfaceCapabilities_);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, nullptr);
		if (formatCount != 0) {
			surfaceFormats_.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface_, &formatCount, surfaceFormats_.data());
		}

		uint32_t modesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, surface_, &modesCount, nullptr);
		if (modesCount != 0) {
			presentModes_.resize(formatCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, surface_, &modesCount, presentModes_.data());
		}
	}

	void VKApi::initDevice() {
		getPhysicalDevice();
		getFamilyIndices();
		getDeviceSwapchainSupport();
		if (deviceGraphicsQueue_ == 0 || devicePresentQueue_ == 0 || surfaceFormats_.empty() || presentModes_.empty()) {
			log_print_err("supported device not found :(");
			abort();
		}

		oak::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<int> uniqueQueueFamilies{ deviceGraphicsQueue_, devicePresentQueue_ };

		float queuePriority = 0.0f;

		for (int queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(deviceQueueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo deviceInfo{};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceInfo.pEnabledFeatures = &deviceFeatures;

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

		VkResult result = vkCreateDevice(physicalDevice_, &deviceInfo, nullptr, &device_);
		if (result != VK_SUCCESS) {
			log_print_err("failed to create device");
			abort();
		}

		vkGetDeviceQueue(device_, deviceGraphicsQueue_, 0, &graphicsQueue_);
		vkGetDeviceQueue(device_, devicePresentQueue_, 0, &presentQueue_);

	}

}
