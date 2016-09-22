#pragma once

#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>

#include "vhandle.h"

struct GLFWwindow;

namespace oak::graphics {

	class VulkanApi {
	public:

		VulkanApi();

		void init(GLFWwindow *window);
		void destroy();
		void update();

	private:
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


		VHandle<VkInstance> instance_;
		VHandle<VkDebugReportCallbackEXT> debugReport_;
		VHandle<VkSurfaceKHR> surface_;
		VkPhysicalDevice physicalDevice_;
		VHandle<VkDevice> device_;
		VkQueue graphicsQueue_;
		VkQueue presentQueue_;
		VHandle<VkSwapchainKHR> swapchain_;
		std::vector<VkImage> swapchainImages_;
		VkFormat swapchainImageFormat_;
		VkExtent2D swapchainExtent_;
		std::vector<VHandle<VkImageView>> swapchainImageViews_;
		VHandle<VkRenderPass> renderPass_;
		VHandle<VkPipelineLayout> pipelineLayout_;
		VHandle<VkPipeline> graphicsPipeline_;
		std::vector<VHandle<VkFramebuffer>> swapchainFramebuffers_;
		VHandle<VkCommandPool> commandPool_;
		std::vector<VkCommandBuffer> commandBuffers_;
		VHandle<VkSemaphore> imageAvaliableSemaphore_;
		VHandle<VkSemaphore> renderFinishedSemaphore_;


		QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device);
		SwapchainSupportDetails getSwapchainSupport(VkPhysicalDevice device);
		VkPhysicalDevice getPhysicalDevice();
		void createShaderModule(const std::vector<char> &code, VHandle<VkShaderModule> &module);

		void initInstance();
		void initDebugReport();
		void initDevice();
		void initSurface(GLFWwindow *window);
		void initSwapchain();
		void initImageViews();
		void initRenderPass();
		void initGraphicsPipeline();
		void initFramebuffers();
		void initCommandPool();
		void initCommandBuffers();
		void initSemaphores();

	};

}