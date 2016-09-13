#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "system.h"


namespace oak {

	class Window : public System {
	public:
		Window(Engine *engine);
		~Window();

		void init() override;
		void destroy() override;

		void update();

	private:
		GLFWwindow *window_;

	};

}