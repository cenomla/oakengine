#pragma once

#include "system.h"

#include <vector>

#include "graphics/renderable.h"
#include "graphics/sprite.h"
#include "gl_buffer.h"
#include "gl_material.h"
#include "gl_vertex_array.h"

namespace oak::graphics {

	class GLRenderer : public System {
	public:
		GLRenderer(Engine &engine);

		void init() override;

		void addObject(const glm::vec3 &position, float rotation, float scale, const Renderable *sprite);

		void render();

	private:
		struct ObjectPos {
			const Renderable *object;
			glm::vec2 position;
			int depth;
			float rotation, scale;

			inline bool operator<(const ObjectPos &other) const { return depth < other.depth; }
		};

		struct Batch {
			size_t start;
			size_t count;
			const GLMaterial *material;
		};

		std::vector<ObjectPos> objects_;
		std::vector<Batch> batches_;

		GLVertexArray vao_;
		GLBuffer vbo_;
		GLBuffer ibo_;
		GLBuffer ubo_;
		
		size_t vertexCount_;
		size_t maxVertices_;
	};

}