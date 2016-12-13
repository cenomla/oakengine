#include "gl_frame_renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "log.h"

namespace oak::graphics {

	GLFrameRenderer::GLFrameRenderer(Engine &engine) : System{ engine, "gl_frame_renderer" } {
		engine_.getEventManager().add<WindowCreateEvent>([this](const WindowCreateEvent& evt){ create(evt.window); });
	}

	void GLFrameRenderer::init() {
		//register callbacks

		engine_.getEventManager().add<WindowResizeEvent>(std::ref(*this));;

	}

	void GLFrameRenderer::create(GLFWwindow *window) {
		//load opengl stuff
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		if (!gladLoadGL()) {
			log::cout << "cannot load gl" << std::endl;
			std::exit(-1);
		}

		log::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;

		glfwGetWindowSize(window, &windowWidth_, &windowHeight_);
		window_ = window;
	}

	void GLFrameRenderer::render() {
		glfwSwapBuffers(window_);
		glViewport(0, 0, windowWidth_, windowHeight_);
		glClearColor(0.3f, 0.2f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			log::cout << "opengl error: " << error << std::endl;
		}
	}

	void GLFrameRenderer::operator()(const WindowResizeEvent& evt) {
		windowWidth_ = evt.width;
		windowHeight_ = evt.height;
	}

}