#pragma once

#include "graphics/api.h"
#include "gl_buffer.h"
#include "gl_framebuffer.h"
#include "gl_shader.h"
#include "gl_texture.h"
#include "gl_vertex_array.h"

struct GLFWwindow;

namespace oak::graphics {

	namespace buffer = GLBuffer;
	namespace shader = GLShader;
	namespace texture = GLTexture;
	namespace framebuffer = GLFramebuffer;
	namespace vertex_array = GLVertexArray;

	class GLApi : public Api {
	public:

		void init() override;
		void terminate() override;

		void swap() override;

	private:
		GLFWwindow *window_;
	};

}