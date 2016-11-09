#pragma once

#include <cinttypes>

#include "system.h"

struct GLFWwindow;

namespace oak {


	class Window : public System {
	public:
		static constexpr uint32_t GL_CONTEXT = 0x00000001;

		Window(Engine &engine, uint32_t flags = 0);
		~Window();

		void init() override;
		void destroy() override;

		void update();

		inline GLFWwindow* getWindowHandle() { return window_; }

	private:
		GLFWwindow *window_;

		uint32_t flags_;

		void createWindow();
		void setCallbacks();

		static void closeCallback(GLFWwindow *window);
		static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
		static void buttonCallback(GLFWwindow *window, int button, int action, int mods);
		static void mouseCallback(GLFWwindow *window, double xpos, double ypos);
		static void resizeCallback(GLFWwindow *window, int width, int height);

	};

}