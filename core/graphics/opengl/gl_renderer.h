#pragma once

#include "graphics/renderer.h"
#include "graphics/batch.h"

#include "gl_vertex_array.h"
#include "gl_buffer.h"

struct GLFWwindow;

namespace oak::graphics {

	class GLRenderer : public Renderer {
	public:

		GLRenderer();

		void init() override;
		void terminate() override;
		void swap() override;
		void render(const Batch& batch) override;
		inline BufferStorage* getStorage() override { return &vbo_; }
		inline BufferStorage* getIStorage() override { return &ibo_; }

	private:
		GLFWwindow *window_;
		GLVertexArray vao_;
		GLBuffer vbo_;
		GLBuffer ibo_;
	};

}