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

		void onWindowResize(int width, int height);

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

		//vulkan handles
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
		VHandle<VkBuffer> vertexBuffer_;
		VHandle<VkDeviceMemory> vertexBufferMemory_;
		VHandle<VkBuffer> indexBuffer_;
		VHandle<VkDeviceMemory> indexBufferMemory_;
		VHandle<VkCommandPool> commandPool_;
		std::vector<VkCommandBuffer> commandBuffers_;
		VHandle<VkSemaphore> imageAvaliableSemaphore_;
		VHandle<VkSemaphore> renderFinishedSemaphore_;

		//api variables
		int windowWidth_, windowHeight_;


		QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device);
		SwapchainSupportDetails getSwapchainSupport(VkPhysicalDevice device);
		VkPhysicalDevice getPhysicalDevice();
		void createShaderModule(const std::vector<char> &code, VHandle<VkShaderModule> &module);
		uint32_t findMemoryType(uint32_t filter, VkMemoryPropertyFlags properties);
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VHandle<VkBuffer> &buffer, VHandle<VkDeviceMemory> &memory);
		void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

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
		void initVertexBuffer();
		void initIndexBuffer();
		void initCommandBuffers();
		void initSemaphores();

		void recreateSwapchain();

	};

}