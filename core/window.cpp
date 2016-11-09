#include "window.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "events.h"
#include "log.h"

namespace oak {

	Window::Window(Engine &engine, uint32_t flags) : System{ engine, "window" }, window_{ nullptr }, flags_{ flags } {
		
	}

	Window::~Window() {

	}

	void Window::init() {
		createWindow();
		setCallbacks();

		TaskManager &tm = engine_.getTaskManager();

		tm.addTask(oak::Task{ [this](){
			this->update();
		}, Task::LOOP_BIT });
	}

	void Window::destroy() {
		glfwDestroyWindow(window_);
		glfwTerminate();
	}

	void Window::update() {
		glfwPollEvents();
		if (glfwWindowShouldClose(window_)) {
			engine_.getEventManager().emitEvent(GameExitEvent{});
		}
	}

	void Window::createWindow() {
		if (!glfwInit()) {
			log::cout << "cannot init glfw" << std::endl;
			std::exit(-1);
		}

		if (flags_ & GL_CONTEXT) {
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		} else {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		window_ = glfwCreateWindow(1280, 720, "Oak Engine", 0, 0);
		if (window_ == nullptr) {
			log::cout << "cannot create window" << std::endl;
			std::exit(-1);
		}

		glfwSetWindowUserPointer(window_, this);

		engine_.getEventManager().emitEvent(WindowCreateEvent{ window_ });

	}

	void Window::setCallbacks() {
		glfwSetWindowCloseCallback(window_, closeCallback);
		glfwSetKeyCallback(window_, keyCallback);
		glfwSetMouseButtonCallback(window_, buttonCallback);
		glfwSetCursorPosCallback(window_, mouseCallback);
		glfwSetWindowSizeCallback(window_, resizeCallback);
	}

	void Window::closeCallback(GLFWwindow *window) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->engine_.getEventManager().emitEvent(GameExitEvent{});
	}

	void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->engine_.getEventManager().emitEvent(KeyEvent{ key, scancode, action, mods });
		if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void Window::buttonCallback(GLFWwindow *window, int button, int action, int mods) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->engine_.getEventManager().emitEvent(ButtonEvent{ button, action, mods });
	}

	void Window::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->engine_.getEventManager().emitEvent(MouseMoveEvent{ static_cast<int>(xpos), static_cast<int>(ypos) });
	}

	void Window::resizeCallback(GLFWwindow *window, int width, int height) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->engine_.getEventManager().emitEvent(WindowResizeEvent{ width, height });
	}

}