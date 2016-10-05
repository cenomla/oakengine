#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace oak::graphics {

	class VDevice {
	public:
		struct QueueFamilyIndices {
			int graphicsFamily = -1;
			int presentFamily = -1;

			bool complete() {
				return graphicsFamily >= 0 && presentFamily >= 0;
			}
		};

		struct SwapchainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> modes;
		};

		VDevice(const VkInstance *instance);
		~VDevice();

		void create(VkSurfaceKHR surface);
		void destroy();

		inline operator const VkPhysicalDevice() const { return physicalDevice_; }
		inline operator const VkDevice() const { return device_; }

		inline VkQueue graphicsQueue() const { return graphicsQueue_; }
		inline VkQueue presentQueue() const { return presentQueue_; }

		inline QueueFamilyIndices getQueueFamilyIndices() const { return familyIndices_; }
		inline const SwapchainSupportDetails& getSwapchainSupportDetails() const { return swapchainDetails_; }
	private:
		const VkInstance *instance_;
		VkPhysicalDevice physicalDevice_;
		VkDevice device_;

		VkQueue graphicsQueue_;
		VkQueue presentQueue_;

		QueueFamilyIndices familyIndices_;
		SwapchainSupportDetails swapchainDetails_;

		void getPhysicalDevice();
		void findQueueFamilyIndices(VkSurfaceKHR surface);
		void getSwapchainSupport(VkSurfaceKHR surface);


	};

}