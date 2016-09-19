#include "window.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "events.h"
#include "log.h"

namespace oak {

	Window::Window(Engine *engine, uint32_t flags) : System{ engine }, window_{ nullptr }, flags_{ flags } {
		
	}

	Window::~Window() {

	}

	void Window::init() {
		createWindow();
		setCallbacks();

		TaskManager &tm = engine_->getTaskManager();

		tm.addTask(Task{ [](){
			glViewport(0, 0, 1280, 720);
			glClearColor(0.3f, 0.2f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}, Task::LOOP_BIT });

		tm.addTask(oak::Task{ [this](){
			this->update();
		}, Task::LOOP_BIT });
	}

	void Window::destroy() {
		glfwDestroyWindow(window_);
		glfwTerminate();
	}

	void Window::update() {
		glfwSwapBuffers(window_);
		glfwPollEvents();
	}

	void Window::createWindow() {
		if (!glfwInit()) {
			log::cout << "cannot init glfw" << std::endl;
			std::exit(-1);
		}

		if (flags_ & USE_VULKAN) {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		} else {
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		}

		window_ = glfwCreateWindow(1280, 720, "Oak Engine", 0, 0);
		if (window_ == nullptr) {
			log::cout << "cannot create window" << std::endl;
			std::exit(-1);
		}

		glfwSetWindowUserPointer(window_, this);

	}

	void Window::setCallbacks() {
		glfwSetWindowCloseCallback(window_, closeCallback);
		glfwSetKeyCallback(window_, keyCallback);
	}

	void Window::closeCallback(GLFWwindow *window) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->engine_->getEventManager().emitEvent(QuitEvent{});
	}

	void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->engine_->getEventManager().emitEvent(KeyEvent{ key, scancode, action, mods });  
	}

	void Window::buttonCallback(GLFWwindow *window, int button, int action, int mods) {

	}

}