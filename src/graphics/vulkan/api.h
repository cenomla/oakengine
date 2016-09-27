#pragma once

#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>

#include "vhandle.h"
#include "vpipeline.h"

struct GLFWwindow;

namespace oak::graphics {


	class VulkanApi {
	public:

#ifdef _DEBUG
		static constexpr uint32_t layerEnabledCount = 1;
#else
		static constexpr uint32_t layerEnabledCount = 0;
#endif

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
		VHandle<VkDescriptorSetLayout> descriptorSetLayout_;
		VPipeline graphicsPipeline_;
		std::vector<VHandle<VkFramebuffer>> swapchainFramebuffers_;
		VHandle<VkImage> depthImage_;
		VHandle<VkDeviceMemory> depthImageMemory_;
		VHandle<VkImageView> depthImageView_;
		VHandle<VkImage> textureImage_;
		VHandle<VkDeviceMemory> textureImageMemory_;
		VHandle<VkImageView> textureImageView_;
		VHandle<VkSampler> textureSampler_;
		VHandle<VkBuffer> vertexBuffer_;
		VHandle<VkDeviceMemory> vertexBufferMemory_;
		VHandle<VkBuffer> indexBuffer_;
		VHandle<VkDeviceMemory> indexBufferMemory_;
		VHandle<VkBuffer> uniformStagingBuffer_;
		VHandle<VkDeviceMemory> uniformStagingBufferMemory_;
		VHandle<VkBuffer> uniformBuffer_;
		VHandle<VkDeviceMemory> uniformBufferMemory_;
		VHandle<VkDescriptorPool> descriptorPool_;
		VkDescriptorSet descriptorSet_;
		VHandle<VkCommandPool> commandPool_;
		std::vector<VkCommandBuffer> commandBuffers_;
		VHandle<VkSemaphore> imageAvaliableSemaphore_;
		VHandle<VkSemaphore> renderFinishedSemaphore_;

		//api variables
		int windowWidth_, windowHeight_;


		QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device);
		SwapchainSupportDetails getSwapchainSupport(VkPhysicalDevice device);
		VkPhysicalDevice getPhysicalDevice();
		uint32_t findMemoryType(uint32_t filter, VkMemoryPropertyFlags properties);
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VHandle<VkBuffer> &buffer, VHandle<VkDeviceMemory> &memory);
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VHandle<VkImage> &image, VHandle<VkDeviceMemory> &memory);
		void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
		void transitionImageLayout(VkImage image, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyImage(VkImage srcImage, VkImage dstImage, uint32_t width, uint32_t height);
		void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VHandle<VkImageView> &imageView);
		VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		void initInstance();
		void initDebugReport();
		void initDevice();
		void initSurface(GLFWwindow *window);
		void initSwapchain();
		void initImageViews();
		void initRenderPass();
		void initDescriptorSetLayout();
		void initGraphicsPipeline();
		void initFramebuffers();
		void initCommandPool();
		void initDepthResource();
		void initTextureImage();
		void initTextureImageView();
		void initTextureSampler();
		void initVertexBuffer();
		void initIndexBuffer();
		void initUniformBuffer();
		void initDescriptorPool();
		void initDescriptorSet();
		void initCommandBuffers();
		void initSemaphores();

		void drawFrame();
		void recreateSwapchain();
		void updateUniformBuffer();

	};

}