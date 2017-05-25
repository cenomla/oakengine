#include "vk_renderer.h"

#include <GLFW/glfw3.h>

#include "log.h"

namespace oak::graphics {

	void VkRenderer::init() {

		if (!glfwVulkanSupported()) {
			log_print_err("vulkan is not supported");
			abort();
		}

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

	void VkRenderer::terminate() {
		vkDestroyInstance(instance_, nullptr);
	}

	void VkRenderer::render(const Batch& batch) {

	}

	void VkRenderer::swap() {

	}

}