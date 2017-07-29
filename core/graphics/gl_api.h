#pragma once

#include "gl_buffer.h"
#include "gl_framebuffer.h"
#include "gl_shader.h"
#include "gl_texture.h"
#include "gl_vertex_array.h"

#include "api.h"
#include "buffer_storage.h"

struct GLFWwindow;

namespace oak::graphics {

	struct Batch;

	namespace buffer = GLBuffer;
	namespace shader = GLShader;
	namespace texture = GLTexture;
	namespace framebuffer = GLFramebuffer;
	namespace vertex_array = GLVertexArray;

	class GLApi : public Api {
	public:

		void init() override;
		void terminate() override;
		
		void setState(const Api::State& state) override;

		void clear(bool color, bool depth, bool stencil) override;
		void draw(const Batch& batch) override;
		void swap() override;

	private:
		GLFWwindow *window_;
		Api::State currentState_;
	};

}