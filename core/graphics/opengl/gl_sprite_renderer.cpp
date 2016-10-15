#include "gl_sprite_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"

namespace oak::graphics {

	GLSpriteRenderer::GLSpriteRenderer(Engine &engine) : System{ engine, "gl_sprite_renderer" }, vbo_{ GL_ARRAY_BUFFER }, ibo_{ GL_ELEMENT_ARRAY_BUFFER }, ubo_{ GL_UNIFORM_BUFFER }, texture_{ GL_TEXTURE_2D }, maxSpriteCount_{ 0 } {

	}

	void GLSpriteRenderer::init() {
		//create a material
		texture_.create("res/textures/tex.png");
		shader_.create("core/graphics/shaders/pass2d/opengl.vert", "core/graphics/shaders/pass2d/opengl.frag");
		shader_.bindBlockIndex("MatrixBlock", 0);

		vao_.create();
		vao_.bind();
		
		vbo_.create();
		vbo_.bind();
		vao_.attributeDescription(
			{0, 0, sizeof(graphics::Sprite::Vertex)}, {
				{0, 0, 2, offsetof(graphics::Sprite::Vertex, graphics::Sprite::Vertex::position)},
				{0, 1, 2, offsetof(graphics::Sprite::Vertex, graphics::Sprite::Vertex::texCoords)},
			});
		vbo_.unbind();

		ibo_.create();
		ibo_.bind();
		
		vao_.unbind();

		engine_.getTaskManager().addTask({
			[this]() {
				render();
			},
			Task::LOOP_BIT
		});

		ubo_.create();
		ubo_.bindBufferBase(0);
	}

	void GLSpriteRenderer::addSprite(const glm::vec3 &position, int animFrameX, int animFrameY, float scale, float rot, const graphics::Sprite *sprite) {
		sprites_.push_back({ (static_cast<uint64_t>(static_cast<int>(position.z)) << 32) | sprite->getMaterialId(), glm::vec2{ position }, animFrameX, animFrameY, scale, rot, sprite });
	}

	struct UniformBufferObject {
		glm::mat4 proj;
		glm::mat4 view;
		glm::mat4 model;
	};

	void GLSpriteRenderer::render() {
		if (sprites_.empty()) { return; }
		//sort sprites by layer and material
		std::sort(std::begin(sprites_), std::end(sprites_));

		//stream buffer
		{
			vbo_.bind();
			vbo_.bufferData(glm::max(sprites_.size(), maxSpriteCount_) * 64, nullptr, GL_STREAM_DRAW);
			void *buffer = vbo_.map(GL_WRITE_ONLY);
			//a batch is a range of sprites with the same material
			size_t index = 0;
			Batch currentBatch{ index, 0 }; //first batch 
			uint32_t id = sprites_.at(0).sprite->getMaterialId();//first sprites material
			//iterate through the sorted sprites
			for (const auto& sp : sprites_) {
				//if the material is different use a different batch
				if (id != sp.sprite->getMaterialId()) {
					index += currentBatch.count;
					batches_.push_back(currentBatch);
					currentBatch = Batch{ index, 0 };
					id = sp.sprite->getMaterialId();
				}
				//stream data
				sp.sprite->draw(buffer, sp.position.x, sp.position.y, sp.animFrameX, sp.animFrameY, sp.scale, sp.rot);
				buffer = static_cast<char*>(buffer) + 64;

				currentBatch.count++;
			}
			batches_.push_back(currentBatch);
			vbo_.unmap();
			vbo_.unbind();
		}

		//grow element array if nessesary
		if (sprites_.size() > maxSpriteCount_) {
			maxSpriteCount_ = sprites_.size();
			ibo_.bind();
			ibo_.bufferData(maxSpriteCount_ * 24, nullptr, GL_STATIC_DRAW);
			uint32_t* buffer = static_cast<uint32_t*>(ibo_.map(GL_WRITE_ONLY));
			uint32_t index = 0;

			for (size_t i = 0; i < sprites_.size() * 6; ) {
				buffer[i++] = index;
				buffer[i++] = index + 3;
				buffer[i++] = index + 2;
				buffer[i++] = index + 2;
				buffer[i++] = index + 1;
				buffer[i++] = index + 0;
				index += 4;
			}

			ibo_.unmap();
			ibo_.unbind();
		}


		UniformBufferObject ubo;


		ubo.proj = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, 1.0f, -1.0f);
		ubo.view = glm::mat4{1.0f};
		ubo.model = glm::mat4{1.0f};

		ubo_.bind();
		ubo_.bufferData(sizeof(ubo), &ubo, GL_STREAM_DRAW);
		ubo_.unbind();

		//draw sprites
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

		vao_.bind();

		for (const auto &batch : batches_) {
			//batch.material->bind(); TODO: add per sprite materials
			texture_.bind(GL_TEXTURE0);
			shader_.bind();
			glDrawElements(GL_TRIANGLES, batch.count * 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.start * 24));
		}
		vao_.unbind();

		sprites_.clear();
		batches_.clear();
	}

}