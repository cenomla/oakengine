#include "window.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "config.h"
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
	}

	void Window::destroy() {
		glfwDestroyWindow(window_);
		glfwTerminate();
	}

	void Window::update() {
		glfwPollEvents();
		if (glfwWindowShouldClose(window_)) {
			engine_.getEventManager().emitEvent(QuitEvent{});
		}
	}

	void Window::createWindow() {
		if (!glfwInit()) {
			log_print_err("cannot init glfw");
			abort();
		}

		if (flags_ & GL_CONTEXT) {
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config::GL_VERSION_MAJOR);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config::GL_VERSION_MINOR);
		} else {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		window_ = glfwCreateWindow(1280, 720, "Oak Engine", 0, 0);
		if (window_ == nullptr) {
			log_print_err("cannot create window");
			abort();
		}

		glfwSetWindowUserPointer(window_, this);

		engine_.getEventManager().emitEvent(WindowCreateEvent{ window_ });
	}

	static void closeCallback(GLFWwindow *window) {
		Engine::inst().getEventManager().emitEvent(QuitEvent{});
	}

	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		Engine::inst().getEventManager().emitEvent(KeyEvent{ key, scancode, action, mods });
	}

	static void buttonCallback(GLFWwindow *window, int button, int action, int mods) {
		Engine::inst().getEventManager().emitEvent(ButtonEvent{ button, action, mods });
	}

	static void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
		Engine::inst().getEventManager().emitEvent(MouseMoveEvent{ static_cast<int>(xpos), static_cast<int>(ypos) });
	}

	static void charCallback(GLFWwindow *window, uint32_t codepoint) {
		Engine::inst().getEventManager().emitEvent(CharEvent{ codepoint });
	}

	static void resizeCallback(GLFWwindow *window, int width, int height) {
		Engine::inst().getEventManager().emitEvent(WindowResizeEvent{ width, height });
	}

	void Window::setCallbacks() {
		glfwSetWindowCloseCallback(window_, closeCallback);
		glfwSetKeyCallback(window_, keyCallback);
		glfwSetMouseButtonCallback(window_, buttonCallback);
		glfwSetCursorPosCallback(window_, mouseCallback);
		glfwSetCharCallback(window_, charCallback);
		glfwSetWindowSizeCallback(window_, resizeCallback);
	}

}