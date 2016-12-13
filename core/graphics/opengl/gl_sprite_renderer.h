#pragma once

#include "system.h"

#include <vector>

#include "graphics/sprite.h"
#include "gl_buffer.h"
#include "gl_material.h"
#include "gl_vertex_array.h"

namespace oak::graphics {

	class GLSpriteRenderer : public System {
	public:
		GLSpriteRenderer(Engine &engine);

		void init() override;

		void addSprite(const glm::vec3 &position, int animFrameX, int animFrameY, float scale, float rot, const graphics::Sprite *sprite);

		void render();

	private:
		struct SpritePos {
			const graphics::Sprite *sprite;
			glm::vec2 position;
			int depth;
			short animFrameX, animFrameY;
			float scale, rot;

			inline bool operator<(const SpritePos &other) const { return depth < other.depth; }
		};

		struct Batch {
			size_t start;
			size_t count;
			const GLMaterial *material;
		};

		std::vector<SpritePos> sprites_;
		std::vector<Batch> batches_;

		graphics::GLVertexArray vao_;
		graphics::GLBuffer vbo_;
		graphics::GLBuffer ibo_;
		graphics::GLBuffer ubo_;
		
		size_t maxSpriteCount_;
	};

}