#pragma once

#include "system.h"

#include <functional>

#include "container.h"
#include "graphics/renderable.h"
#include "graphics/vertex.h"
#include "gl_buffer.h"
#include "gl_material.h"
#include "gl_vertex_array.h"

namespace oak::graphics {

	struct Batch {
		const GLMaterial *material;
		size_t start, count, objectCount;
		uint32_t layer;
	};

	class GLRenderer : public System {
	public:
		GLRenderer();

		void init() override;

		void run() override;

		void addObject(const glm::vec2 &position, float depth, uint32_t layer, float rotation, float scale, const Renderable *object);
		void setDrawOp(uint32_t layer, const std::function<void(const GLVertexArray&, const Batch&)> &op);

	private:
		struct ObjectPos {
			const Renderable *object;
			glm::vec2 position;
			float depth; 
			uint32_t layer;
			float rotation, scale;

			bool operator<(const ObjectPos &other) const { 
				return layer == other.layer ? 
					depth < other.depth : 
					layer < other.layer; 
			}
		};

		oak::vector<ObjectPos> objects_;
		oak::vector<Batch> batches_;
		oak::vector<std::function<void(const GLVertexArray&, const Batch&)>> drawOperations_;

		GLVertexArray vao_;
		GLBuffer vbo_;
		GLBuffer ibo_;
		
		size_t vertexCount_;
		size_t maxVertices_;
	};

}