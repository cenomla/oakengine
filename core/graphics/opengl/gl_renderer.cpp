#include "gl_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resource_manager.h"
#include "engine.h"

namespace oak::graphics {

	GLRenderer::GLRenderer(Engine &engine) : System{ engine, "gl_sprite_renderer" }, vbo_{ GL_ARRAY_BUFFER }, ibo_{ GL_ELEMENT_ARRAY_BUFFER }, vertexCount_{ 0 }, maxVertices_{ 0 } {

	}

	void GLRenderer::init() {
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
	}

	void GLRenderer::addObject(const glm::vec3 &position, float rotation, float scale, const Renderable *object) {
		objects_.push_back({ object, glm::vec2{ position }, static_cast<int>(position.z), rotation, scale });
		vertexCount_ += object->getVertexCount();
	}

	void GLRenderer::render() {
		if (objects_.empty()) { return; }
		//sort sprites by layer and material
		std::sort(std::begin(objects_), std::end(objects_));

		//stream buffer
		{
			vbo_.bind();
			vbo_.bufferData(glm::max(maxVertices_, vertexCount_) * sizeof(Sprite::Vertex), nullptr, GL_STREAM_DRAW);
			void *buffer = vbo_.map(GL_WRITE_ONLY);
			//a batch is a range of object with the same material
			size_t index = 0;
			size_t id = objects_.at(0).object->getMaterialId();//first objects material
			Batch currentBatch{ index, 0, &Engine::inst().getSystem<ResourceManager>().require<GLMaterial>(id) }; //first batch
			//iterate through the sorted object
			for (const auto& op : objects_) {
				//if the material is different use a different batch
				if (id != op.object->getMaterialId()) {
					index += currentBatch.count;
					batches_.push_back(currentBatch);
					id = op.object->getMaterialId();
					currentBatch = Batch{ index, 0, &Engine::inst().getSystem<ResourceManager>().require<GLMaterial>(id) };
				}
				//stream data
				op.object->draw(buffer, op.position.x, op.position.y, op.rotation, op.scale);
				buffer = static_cast<char*>(buffer) + op.object->getVertexCount() * sizeof(Sprite::Vertex);

				currentBatch.count += op.object->getVertexCount() / 4;
			}
			batches_.push_back(currentBatch);
			vbo_.unmap();
			vbo_.unbind();
		}

		//grow element array if nessesary
		if (vertexCount_ >  maxVertices_) {
			maxVertices_ = vertexCount_;
			ibo_.bind();
			ibo_.bufferData(maxVertices_ * 6, nullptr, GL_STATIC_DRAW);
			uint32_t* buffer = static_cast<uint32_t*>(ibo_.map(GL_WRITE_ONLY));
			uint32_t index = 0;

			for (size_t i = 0; i < maxVertices_ / 4 * 6; ) {
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

		//draw sprites
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

		vao_.bind();

		for (const auto &batch : batches_) {
			batch.material->bind();
			glDrawElements(GL_TRIANGLES, batch.count * 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.start * 24));
		}
		vao_.unbind();

		vertexCount_ = 0;

		objects_.clear();
		batches_.clear();
	}

}