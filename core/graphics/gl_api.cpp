#include "gl_api.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <event_manager.h>
#include <input_manager.h>
#include <input_events.h>
#include <log.h>

#include "material.h"
#include "batch.h"

namespace oak::graphics {

	static void post_gl_call(const char *name, void *funcptr, int len_args, ...) {
		GLenum error = glad_glGetError();
		if (error != GL_NO_ERROR) {
			log_print_warn("opengl error: %i, %s", error, name);
		}
	}

	static void change_gl_state(const Api::State& state, const Api::State& change) {
		//viewport
		if (state.viewport != change.viewport) {
			glViewport(change.viewport.x, change.viewport.y, change.viewport.width, change.viewport.height);
		}
		//scissor test
		if (state.scissorRect != change.scissorRect) {
			if (change.scissorRect.empty()) {
				glDisable(GL_SCISSOR_TEST);
			} else {
				if (state.scissorRect.empty()) {
					glEnable(GL_SCISSOR_TEST);
				}
				glScissor(change.scissorRect.x, change.scissorRect.y, change.scissorRect.width, change.scissorRect.height);
			}
		}
		//depth test
		if (state.depthOp != change.depthOp) {
			if (change.depthOp == BoolOp::NONE) {
				glDisable(GL_DEPTH_TEST);
			} else {
				if (state.depthOp == BoolOp::NONE) {
					glEnable(GL_DEPTH_TEST);
				}
				glDepthFunc(GL_NEVER + static_cast<int>(change.depthOp));
			}
		}

		//face culling
		if (state.faceCull != change.faceCull) {
			if (change.faceCull == FaceCull::NONE) {
				glDisable(GL_CULL_FACE);
			} else {
				if (state.faceCull == FaceCull::NONE) {
					glEnable(GL_CULL_FACE);
				}
				glCullFace(GL_FRONT + static_cast<int>(change.faceCull));
			}
		}

		//blend mode
		if (state.blendMode != change.blendMode) {
			if (change.blendMode == BlendMode::NONE) {
				glDisable(GL_BLEND);
			} else {
				if (state.blendMode == BlendMode::NONE) {
					glEnable(GL_BLEND);
				}

				switch(change.blendMode) {
					case BlendMode::NORMAL:
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						break;
					case BlendMode::ADD:
						glBlendFunc(GL_ONE, GL_ONE);
						break;
					case BlendMode::MULTIPLY:
						glBlendFunc(GL_DST_COLOR, GL_ZERO);
						break;
					case BlendMode::NONE:
						break;
				}
			}
		}

		//draw mask
		if (state.drawMask.red != change.drawMask.red || 
			state.drawMask.green != change.drawMask.green || 
			state.drawMask.blue != change.drawMask.blue || 
			state.drawMask.alpha != change.drawMask.alpha) {
			glColorMask(change.drawMask.red, change.drawMask.green, change.drawMask.blue, change.drawMask.alpha);
		}

		if (state.drawMask.depth != change.drawMask.depth) {
			glDepthMask(change.drawMask.depth);
		}

		if (state.drawMask.stencil != change.drawMask.stencil) {
			glStencilMask(change.drawMask.stencil);
		}

		//clear values
		if (state.clearColor != change.clearColor) {
			glClearColor(change.clearColor.r, change.clearColor.g, change.clearColor.b, change.clearColor.a);
		}
		if (state.clearDepth != change.clearDepth) {
			glClearDepth(change.clearDepth);
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

		currentState_ = Api::State{
			glm::vec4{ 0.0f },
			1.0f,
			BoolOp::NONE,
			View{},
			MaskState{},
			FaceCull::NONE,
			View{},
			BlendMode::NONE
		};

		EventManager::inst().getQueue<WindowCreateEvent>().emit({ window_ });
		InputManager::inst().update();
	}

	void GLApi::terminate() {
		glfwDestroyWindow(window_);
	}

	void GLApi::setState(const Api::State& state) {
		change_gl_state(currentState_, state);
		currentState_ = state;
	}

	void GLApi::clear(bool color, bool depth, bool stencil) {
		glClear((color ? GL_COLOR_BUFFER_BIT : 0) | (depth ? GL_DEPTH_BUFFER_BIT : 0) | (stencil ? GL_STENCIL_BUFFER_BIT : 0));
	}

	void GLApi::draw(const Batch& batch) {
		//bind material
		oak::graphics::shader::bind(*batch.material->shader);
		for (int i = 0; i < 16; i++) {
			if (batch.material->textures[i] != nullptr) {
				oak::graphics::texture::bind(*batch.material->textures[i], i);
			}
		}
		batch.storage->bind();
		//render stuff

		if (batch.flags & Batch::INDEX) {
			if (batch.instances > 0) {
				glDrawElementsInstanced(((batch.flags & Batch::DRAW_MASK) >> 1) - 1, batch.count, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.offset * 4), batch.instances);
			} else {
				glDrawElements(((batch.flags & Batch::DRAW_MASK) >> 1) - 1, batch.count, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.offset * 4));
			}
		} else {
			glDrawArrays(((batch.flags & Batch::DRAW_MASK) >> 1) - 1, batch.offset, batch.count);
		}
	}

	void GLApi::swap() {
		glfwSwapBuffers(window_);
	}

}