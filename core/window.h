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

	};

}