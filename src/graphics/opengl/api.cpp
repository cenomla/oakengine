#include "api.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "log.h"

namespace oak::graphics {

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
	}

	void OpenglApi::destroy() {
		
	}

	void OpenglApi::update() {
		glfwSwapBuffers(window_);
		glViewport(0, 0, 1280, 720);
		glClearColor(0.3f, 0.2f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

}