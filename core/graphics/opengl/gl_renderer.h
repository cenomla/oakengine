#pragma once

#include <functional>

#include "graphics/renderer.h"
#include "graphics/batch.h"
#include "container.h"

#include "gl_buffer.h"
#include "gl_vertex_array.h"

namespace oak::graphics {

	class GLRenderer : public Renderer {
	public:
		GLRenderer();

		void init() override;
		void render(const void *data, const Batch& batch) override;

	private:
		GLVertexArray vao_;
		GLBuffer vbo_;
		GLBuffer ibo_;
	};

}