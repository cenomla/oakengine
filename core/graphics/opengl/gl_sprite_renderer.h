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
			//ff ff ff ff ff ff ff ff
			//32 bit layer, 32 bit material
			uint64_t id;
			glm::vec2 position;
			int animFrameX, animFrameY;
			float scale, rot;
			const graphics::Sprite *sprite;

			inline bool operator<(const SpritePos &other) const { return id < other.id; }
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