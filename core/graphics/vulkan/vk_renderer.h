#pragma once

#include <vulkan/vulkan.h>

#include "graphics/renderer.h"

struct GLFWwindow;

namespace oak::graphics {

	class VkRenderer : public Renderer {
	public:

		void init() override;
		void terminate() override;
		void setPipeline(Pipeline *pipeline) override;
		void render() override;
	private:
		GLFWwindow *window_;
		VkInstance instance_;
		VkDebugReportCallbackEXT debugReport_;
		VkSurfaceKHR surface_;
		VkPhysicalDevice physicalDevice_;
		int deviceGraphicsQueue_;
		int devicePresentQueue_;
		VkSurfaceCapabilitiesKHR surfaceCapabilities_;
		oak::vector<VkSurfaceFormatKHR> surfaceFormats_;
		oak::vector<VkPresentModeKHR> presentModes_;
		VkDevice device_;
		VkQueue graphicsQueue_;
		VkQueue presentQueue_;

		void initInstance();
		void initDebugReport();
		void getPhysicalDevice();
		void getFamilyIndices();
		void getDeviceSwapchainSupport();
		void initDevice();
	};

}