#pragma once

#include "graphics/renderer.h"
#include "graphics/batch.h"

#include "gl_vertex_array.h"
#include "gl_buffer.h"

namespace oak::graphics {

	class GLRenderer : public Renderer {
	public:

		GLRenderer();

		void init() override;
		void render(const Batch& batch) override;
		inline BufferStorage* getStorage() override { return &vbo_; }

	private:
		GLVertexArray vao_;
		GLBuffer vbo_;
	};

}