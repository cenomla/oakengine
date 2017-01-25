#include "gl_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resource_manager.h"
#include "view_system.h"
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

	void GLRenderer::addObject(const glm::vec2 &position, float depth, uint32_t layer, float rotation, float scale, const Renderable *object) {
		size_t vCount = object->getVertexCount();
		if (vCount > 0) {
			vertexCount_ += vCount;
			objects_.push_back({ object, position, depth, layer, rotation, scale });
		}
	}

	void GLRenderer::render() {
		//if there are no objects dont do anything
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
			size_t id = objects_.at(0).object->getMaterialId();
			uint32_t layer = objects_.at(0).layer;

			auto& resManager = Engine::inst().getSystem<ResourceManager>();
			Batch currentBatch{ &resManager.require<GLMaterial>(id), index, 0, layer }; //first batch
			//iterate through the sorted object
			for (const auto& it : objects_) {
				//if the material is different use a different batch
				if (id != it.object->getMaterialId() || layer != it.layer) {
					index += currentBatch.count;
					batches_.push_back(currentBatch);
					id = it.object->getMaterialId();
					layer = it.layer;
					currentBatch = Batch{ &resManager.require<GLMaterial>(id), index, 0, layer };
				}
				//stream data
				it.object->draw(buffer, it.position.x, it.position.y, it.rotation, it.scale);
				buffer = static_cast<char*>(buffer) + it.object->getVertexCount() * sizeof(Sprite::Vertex);

				currentBatch.count += it.object->getVertexCount() / 4;
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

		auto &viewSystem = Engine::inst().getSystem<ViewSystem>();

		for (const auto &batch : batches_) {
			batch.material->shader->bindBlockIndex("MatrixBlock", viewSystem.getViewId(batch.layer));
			batch.material->bind();
			glDrawElements(GL_TRIANGLES, batch.count * 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(batch.start * 24));
		}
		vao_.unbind();

		vertexCount_ = 0;

		objects_.clear();
		batches_.clear();
	}

}