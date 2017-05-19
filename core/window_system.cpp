#include "window_system.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "event_manager.h"
#include "input_events.h"
#include "log.h"

namespace oak {

	WindowSystem::WindowSystem(const oak::string& title) : title_{ title } {}

	void WindowSystem::init() {
		createWindow();
		setCallbacks();
	}

	void WindowSystem::terminate() {
		glfwDestroyWindow(window_);
		glfwTerminate();
	}

	void WindowSystem::run() {
		glfwSwapBuffers(window_);
		glViewport(0, 0, 1280, 720);
		glClearColor(0.3f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			log_print_warn("opengl error: %i", error);
		}

		glfwPollEvents();
	}

	void WindowSystem::createWindow() {
		if (!glfwInit()) {
			log_print_err("cannot init glfw");
			abort();
		}

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

		window_ = glfwCreateWindow(1280, 720, title_.c_str(), 0, 0);
		if (window_ == nullptr) {
			log_print_err("cannot create window");
			abort();
		}

		glfwSetWindowUserPointer(window_, this);

		glfwMakeContextCurrent(window_);
		glfwSwapInterval(1);

		if (!gladLoadGL()) {
			log_print_err("cannot load gl");
			abort();
		}

		log_print_out("opengl version: %s", glGetString(GL_VERSION));

		EventManager::inst().getQueue<WindowCreateEvent>().emit({});
	}

	static void closeCallback(GLFWwindow *window) {
		EventManager::inst().getQueue<WindowCloseEvent>().emit({});
	}

	static void resizeCallback(GLFWwindow *window, int width, int height) {
		EventManager::inst().getQueue<WindowResizeEvent>().emit({ width, height });
	}

	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		EventManager::inst().getQueue<KeyEvent>().emit({ key, scancode, action, mods });
	}

	static void buttonCallback(GLFWwindow *window, int button, int action, int mods) {
		EventManager::inst().getQueue<ButtonEvent>().emit({ button, action, mods });
	}

	static void cursorCallback(GLFWwindow *window, double xpos, double ypos) {
		EventManager::inst().getQueue<CursorEvent>().emit({ static_cast<float>(xpos), static_cast<float>(ypos) });
	}

	static void charCallback(GLFWwindow *window, uint32_t codepoint) {
		EventManager::inst().getQueue<TextEvent>().emit(TextEvent{ codepoint });
	}

	void WindowSystem::setCallbacks() {
		glfwSetWindowCloseCallback(window_, closeCallback);
		glfwSetWindowSizeCallback(window_, resizeCallback);
		glfwSetKeyCallback(window_, keyCallback);
		glfwSetMouseButtonCallback(window_, buttonCallback);
		glfwSetCursorPosCallback(window_, cursorCallback);
		glfwSetCharCallback(window_, charCallback);
	}

}
