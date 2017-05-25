#include "gl_renderer.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics/material.h"
#include "event_manager.h"
#include "input_events.h"
#include "log.h"

namespace oak::graphics {

	GLRenderer::GLRenderer() : vbo_{ GL_ARRAY_BUFFER }, ibo_{ GL_ELEMENT_ARRAY_BUFFER } {}

	static void post_gl_call(const char *name, void *funcptr, int len_args, ...) {
		GLenum error = glad_glGetError();
		if (error != GL_NO_ERROR) {
			log_print_warn("opengl error: %i, %s", error, name);
		}
	}

	void GLRenderer::init() {
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

		window_ = glfwCreateWindow(1280, 720, "sandbox", 0, 0);
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

#ifdef GLAD_DEBUG
		glad_set_post_callback(post_gl_call);
#endif

		log_print_out("opengl version: %s", glGetString(GL_VERSION));
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		EventManager::inst().getQueue<WindowCreateEvent>().emit({ window_ });


		vao_.create();
		vbo_.create();
		vao_.bind();
		ibo_.create();
		ibo_.bind();
		vao_.unbind();
	}

	void GLRenderer::terminate() {
		glfwDestroyWindow(window_);
	}

	void GLRenderer::swap() {
		glfwSwapBuffers(window_);
		glViewport(0, 0, 1280, 720);
		glClearColor(0.3f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GLRenderer::render(const Batch& batch) {
		
		vao_.bind();
		vbo_.bind();
		vao_.attributeDescription(batch.desc.stride, batch.desc.attributes);
		vbo_.unbind();

		glUseProgram(batch.material->shader->id);
		glBindTexture(GL_TEXTURE_2D, batch.material->texture->id);

		glDrawElements(GL_TRIANGLES, batch.count, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.offset * 4));

		vao_.unbind();
	}

}