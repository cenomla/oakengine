#pragma once

#include "engine.h"
#include "system.h"

struct GLFWwindow;

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