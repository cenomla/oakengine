#include "api.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "log.h"

namespace oak::graphics {

	void OpenglApi::init(GLFWwindow *window) {
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		if (!gladLoadGL()) {
			log::cout << "cannot load gl" << std::endl;
			std::exit(-1);
		}

		log::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;
	}

	void OpenglApi::destroy() {
		
	}

}