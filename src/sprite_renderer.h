#pragma once

#include "system.h"

#include <vector>
#include <glm/glm.hpp>

#include "graphics/opengl/glvertex_array.h"
#include "graphics/opengl/glbuffer.h"
#include "graphics/opengl/glpipeline.h"
#include "graphics/opengl/gltexture.h"
#include "graphics/opengl/glshader.h"
#include "graphics/sprite.h"

namespace oak {


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
	};

	class SpriteRenderer : public System {
	public:
		SpriteRenderer(Engine *engine);
		~SpriteRenderer();

		void init() override;
		void destroy() override;

		void addSprite(const glm::vec3 &position, int animFrameX, int animFrameY, float scale, float rot, const graphics::Sprite *sprite);

		void render();

	private:

		std::vector<SpritePos> sprites_;
		std::vector<Batch> batches_;
		graphics::GLVertexArray vao_;
		graphics::GLBuffer vbo_;
		graphics::GLBuffer ibo_;
		graphics::GLBuffer ubo_;
		graphics::GLShader shader_;
		graphics::GLTexture texture_;
		graphics::Material<graphics::GLPipeline, graphics::GLTexture> *material_;
		size_t maxSpriteCount_;
	};

}