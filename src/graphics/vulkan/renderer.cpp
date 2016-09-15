#include "renderer.h"

#include <GLFW/glfw3.h>

#include "log.h"

namespace oak::graphics::vulkan {

	void Renderer::init() {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "hello app";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "oak engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		

		VkInstanceCreateInfo instanceInfo{};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwInit();

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			log::cout << glfwExtensions[i] << std::endl;
		}

		instanceInfo.enabledExtensionCount = glfwExtensionCount;
		instanceInfo.ppEnabledExtensionNames = glfwExtensions;

		instanceInfo.enabledLayerCount = 0;

		VkInstance instance;
		VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);

		if (result != VK_SUCCESS) {
			log::cout << "couldnt create vulkan instance" << std::endl;
			std::exit(-1);
		}

		uint32_t extensionCount = 0;
		std::vector<VkExtensionProperties> extensions;

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		extensions.resize(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, &extensions[0]);

		for (auto it : extensions) {
			log::cout << it.extensionName << std::endl;
		}

	}

}