#pragma once

#include "system.h"
#include "events.h"

struct GLFWwindow;

namespace oak::graphics {

	class GLFrameRenderer : public System {
	public:
		GLFrameRenderer(Engine &engine);

		void init() override;
		void create(GLFWwindow *window);

		void update();

		void operator()(const WindowResizeEvent& evt);
	private:
		GLFWwindow *window_;
		int windowWidth_, windowHeight_;

	};

}