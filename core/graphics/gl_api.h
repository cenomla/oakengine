#pragma once

#include "gl_buffer.h"
#include "gl_framebuffer.h"
#include "gl_shader.h"
#include "gl_texture.h"
#include "gl_vertex_array.h"

#include "api.h"
#include "batch.h"
#include "buffer_storage.h"

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

		void pushState();
		void popState();
		void draw(const oak::vector<Batch>& batches) override;
		void setView();
		void setBlend();

		void swap() override;

	private:
		GLFWwindow *window_;
	};

}