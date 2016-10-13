#include "vdevice.h"


#include <set>
#include "log.h"

namespace oak::graphics {

	VDevice::VDevice(const VkInstance *instance) : instance_{ instance }, device_{ VK_NULL_HANDLE } {

	}

	VDevice::~VDevice() {
		destroy();
	}

	void VDevice::create(VkSurfaceKHR surface) {
		if (device_ != VK_NULL_HANDLE) { return; }

		getPhysicalDevice();
		findQueueFamilyIndices(surface);
		getSwapchainSupport(surface);

		if (!familyIndices_.complete() || swapchainDetails_.formats.empty() || swapchainDetails_.modes.empty()) {
			log::cout << "no supported devices found" << std::endl;
			std::exit(-1);
		}

		//create queue family create infos
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<int> uniqueQueueFamilies{ familyIndices_.graphicsFamily, familyIndices_.presentFamily };

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
		VkResult result = vkCreateDevice(physicalDevice_, &deviceInfo, nullptr, &device_);
		if (result != VK_SUCCESS) {
			log::cout << "failed to create device" << std::endl;
			std::exit(-1);
		}
		//get the queues from the device;
		vkGetDeviceQueue(device_, familyIndices_.graphicsFamily, 0, &graphicsQueue_);
		vkGetDeviceQueue(device_, familyIndices_.presentFamily, 0, &presentQueue_);

	}

	void VDevice::destroy() {
		if (device_ != VK_NULL_HANDLE) {
			vkDestroyDevice(device_, nullptr);
			device_ = VK_NULL_HANDLE;
		}
	}

	void VDevice::getPhysicalDevice() {
		//enumerate physical devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(*instance_, &deviceCount, nullptr);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(*instance_, &deviceCount, devices.data());

		//print amount of devices
		log::cout << "device count: " << deviceCount << std::endl;

		//check for a suitable device
		for (const auto &device : devices) {
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


			log::cout << "device name: " << deviceProperties.deviceName << std::endl;
			//physicalDevice_ = device;
		}
		//just kidding, going with the first device
		physicalDevice_ = devices[0];
	}

	void VDevice::findQueueFamilyIndices(VkSurfaceKHR surface) {		
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamilies.data());

		log::cout << "queue family count: " << queueFamilyCount << std::endl;

		int i = 0;
		for (const auto &queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				familyIndices_.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice_, i, surface, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport) {
				familyIndices_.presentFamily = i;
			}

			if (familyIndices_.complete()) {
				break;
			}

			i++;
		}
	}

	void VDevice::getSwapchainSupport(VkSurfaceKHR surface) {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice_, surface, &swapchainDetails_.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface, &formatCount, nullptr);
		if (formatCount != 0) {
			swapchainDetails_.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface, &formatCount, swapchainDetails_.formats.data());
		}

		uint32_t modesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, surface, &modesCount, nullptr);
		if (modesCount != 0) {
			swapchainDetails_.modes.resize(modesCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice_, surface, &modesCount, swapchainDetails_.modes.data());
		}
	}

}