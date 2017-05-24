#pragma once

#include <functional>

#include "graphics/renderer.h"
#include "graphics/batch.h"
#include "container.h"

#include "gl_vertex_array.h"

namespace oak::graphics {

	class GLRenderer : public Renderer {
	public:

		void init() override;
		void render(BufferStorage *storage, const Batch& batch) override;

	private:
		GLVertexArray vao_;
	};

}