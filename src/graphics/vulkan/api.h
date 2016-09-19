#pragma once

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace oak::graphics {

	class VulkanApi {
	public:

		void init(GLFWwindow *window);
		void destroy();

	private:
		struct QueueFamilyIndices {
			int graphicsFamily = -1;
			int presentFamily = -1;

			bool complete() {
				return graphicsFamily >= 0 && presentFamily >= 0;
			}
		};


		VkInstance instance_;
		VkDebugReportCallbackEXT debugReport_;
		VkSurfaceKHR surface_;
		VkDevice device_;
		VkQueue graphicQueue_;
		VkQueue presentQueue_;

		void initInstance();
		void initDebugReport();
		QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device);
		void initDevice();
		void initSurface(GLFWwindow *window);

	};

}