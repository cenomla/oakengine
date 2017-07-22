#include "gl_api.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics/material.h"
#include "event_manager.h"
#include "input_manager.h"
#include "input_events.h"
#include "log.h"

namespace oak::graphics {

	static void post_gl_call(const char *name, void *funcptr, int len_args, ...) {
		GLenum error = glad_glGetError();
		if (error != GL_NO_ERROR) {
			log_print_warn("opengl error: %i, %s", error, name);
		}
	}

	void GLApi::init() {
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();

		const GLFWvidmode *mode = glfwGetVideoMode(monitor);

		/*		
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		*/

		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

		window_ = glfwCreateWindow(mode->width, mode->height, "sandbox", 0, 0);
		if (window_ == nullptr) {
			log_print_err("cannot create window");
			abort();
		}

		glfwMakeContextCurrent(window_);
		glfwSwapInterval(1);

		if (!gladLoadGL()) {
			log_print_err("cannot load gl");
			abort();
		}

#ifdef DEBUG
		glad_set_post_callback(post_gl_call);
#endif

		log_print_out("opengl version: %s", glGetString(GL_VERSION));

		EventManager::inst().getQueue<WindowCreateEvent>().emit({ window_ });
		InputManager::inst().update();
	}

	void GLApi::terminate() {
		glfwDestroyWindow(window_);
	}

	void GLApi::swap() {
		glfwSwapBuffers(window_);
	}

}