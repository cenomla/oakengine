#pragma once

#include <vulkan/vulkan.h>

#include "graphics/renderer.h"

struct GLFWwindow;

namespace oak::graphics {

	class VkRenderer : public Renderer {
	public:

		void init() override;
		void terminate() override;
		void swap() override;
		void render(const Batch& batch) override;
		inline BufferStorage* getStorage() override { return nullptr; }
		inline BufferStorage* getIStorage() override { return nullptr; }
	private:
		GLFWwindow *window_;
		VkInstance instance_;
		VkDebugReportCallbackEXT debugReport_;
		VkSurfaceKHR surface_;
		VkPhysicalDevice physicalDevice_;

		void initInstance();
		void initDebugReport();
		void getPhysicalDevice();
		void initDevice();
	};

}