#include "api.h"

#include <chrono>
#include <fstream>
#include <GLFW/glfw3.h>

#include "graphics/vertex.h"
#include "glshader.h"

#include "log.h"

namespace oak::graphics {

	OpenglApi::OpenglApi() {}

	void OpenglApi::init(GLFWwindow *window) {
		//load opengl stuff
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		if (!gladLoadGL()) {
			log::cout << "cannot load gl" << std::endl;
			std::exit(-1);
		}

		log::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;
		window_ = window;

		glfwGetWindowSize(window_, &windowWidth_, &windowHeight_);
	}

	void OpenglApi::destroy() {
	}

	void OpenglApi::update() {
		glfwSwapBuffers(window_);
		glViewport(0, 0, windowWidth_, windowHeight_);
		glClearColor(0.3f, 0.2f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			log::cout << "opengl error: " << error << std::endl;
		}
	}

	void OpenglApi::onWindowResize(int width, int height) {
		windowWidth_ = width;
		windowHeight_ = height;
	}

}