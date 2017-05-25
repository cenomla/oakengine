#include "vk_renderer.h"

#include <GLFW/glfw3.h>

#include "event_manager.h"
#include "input_events.h"
#include "log.h"

namespace oak::graphics {

	void VkRenderer::init() {
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

		EventManager::inst().getQueue<WindowCreateEvent>().emit({ window_ });

		//init surface
		VkResult result = glfwCreateWindowSurface(instance_, window_, nullptr, &surface_);
		if (result != VK_SUCCESS) {
			log_print_err("failed to create vulkan surface");
			abort();
		}

		initDevice();


	}

	void VkRenderer::terminate() {
		vkDestroyInstance(instance_, nullptr);
	}

	void VkRenderer::render(const Batch& batch) {

	}

	void VkRenderer::swap() {

	}

	void VkRenderer::initInstance() {
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
		//instanceInfo.enabledExtensionCount = 0;
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


	void VkRenderer::initDebugReport() {
		VkDebugReportCallbackCreateInfoEXT debugInfo{};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		debugInfo.pfnCallback = debugCallback;

		auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance_, "vkCreateDebugReportCallbackEXT"));
		func(instance_, &debugInfo, nullptr, &debugReport_);
	}

	void VkRenderer::getPhysicalDevice() {
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

	void VkRenderer::initDevice() {
		getPhysicalDevice();
	}

}