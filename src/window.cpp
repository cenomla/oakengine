#include "window.h"

#include <iostream>

namespace oak {

	Window::Window(Engine &engine) : System{ engine }, window_{ nullptr } {
	}

	Window::~Window() {

	}

	void Window::init() {
		if (!glfwInit()) {
			std::cout << "cannot init glfw" << std::endl;
			std::exit(-1);
		}

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		window_ = glfwCreateWindow(1280, 720, "Oak Engine", 0, 0);
		if (window_ == nullptr) {
			std::cout << "cannot create window" << std::endl;
			std::exit(-1);
		}

		glfwMakeContextCurrent(window_);
		glfwSwapInterval(1);

		if (!gladLoadGL()) {
			std::cout << "cannot load gl" << std::endl;
			std::exit(-1);
		}
	}

	void Window::destroy() {
		glfwDestroyWindow(window_);
		glfwTerminate();
	}

	void Window::update() {
		glfwSwapBuffers(window_);
		glfwPollEvents();
	}

}