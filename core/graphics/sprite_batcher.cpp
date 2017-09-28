#include "sprite_batcher.h"

#include <algorithm>

#include "buffer_storage.h"

#include "log.h"

namespace oak::graphics {

	void SpriteBatcher::init() {
		bufferInfo_.storage.create({ oak::vector<AttributeType>{ 
			AttributeType::POSITION2D,
			AttributeType::UV
		} });
	}

	void SpriteBatcher::terminate() {
		bufferInfo_.storage.destroy();
	}

	void SpriteBatcher::addSprite(uint32_t layer, const Material *material, const Sprite *sprite, const glm::mat3& transform) {
		sprites_.push_back({ layer, material, sprite, transform });
	}

	void SpriteBatcher::run() {
		batches_.clear();
		if (sprites_.empty()) { return; }

		std::sort(std::begin(sprites_), std::end(sprites_));


		//create batches 
		const Material *mat = sprites_[0].material;
		uint32_t layer = sprites_[0].layer;
		Batch currentBatch{ &bufferInfo_.storage, mat, bufferInfo_.offset, 0, layer }; //first batch
		//iterate through the sorted object
		for (auto& it : sprites_) {
			//if the material is different use a different batch
			if (mat != it.material || layer != it.layer) {
				//update mat and layer
				mat = it.material;
				layer = it.layer;
				//offset the buffer layout
				bufferInfo_.offset += currentBatch.count;
				//make a new batch
				batches_.push_back(currentBatch);
				currentBatch = Batch{ &bufferInfo_.storage, mat, bufferInfo_.offset, 0, layer };
			}
			currentBatch.count += 6;
			bufferInfo_.size[0] += 4 * sizeof(Sprite::Vertex); //add to size of buffer total size of mesh
			bufferInfo_.size[1] += 6 * sizeof(uint32_t); //add to size of index buffer total size of indices array
		}
		batches_.push_back(currentBatch);

		//resize buffer
		for (int i = 0; i < 2; i++) {
			if (bufferInfo_.size[i] > bufferInfo_.capacity[i]) {
				bufferInfo_.capacity[i] = bufferInfo_.size[i];
				bufferInfo_.storage.data(i, bufferInfo_.capacity[i], nullptr);
			}
		}

		//fill batches
		//map buffers
		for (int i = 0; i < 2; i++) {
			if (bufferInfo_.size[i] > 0) {
				bufferInfo_.map[i] = bufferInfo_.storage.map(i, BufferAccess::WRITE_ONLY);
			}
		}

		//copy data to buffers
		for (auto& it : sprites_) {
			if (bufferInfo_.map[0]) {
				Sprite::Vertex *vd = static_cast<Sprite::Vertex*>(bufferInfo_.map[0]);
				glm::vec2 center{ it.sprite->centerX, it.sprite->centerY };
				vd[0].position = glm::vec2{ it.transform * glm::vec3{ -center, 1.0f } };
				vd[1].position = glm::vec2{ it.transform * glm::vec3{ -center + glm::vec2{ 0.0f, it.sprite->height }, 1.0f } };
				vd[2].position = glm::vec2{ it.transform * glm::vec3{ -center + glm::vec2{ it.sprite->width, it.sprite->height }, 1.0f } };
				vd[3].position = glm::vec2{ it.transform * glm::vec3{ -center + glm::vec2{ it.sprite->width, 0.0f }, 1.0f } };

				vd[0].uv = it.sprite->region.pos;
				vd[1].uv = it.sprite->region.pos + glm::vec2{ 0.0f, it.sprite->region.extent.y };
				vd[2].uv = it.sprite->region.pos + glm::vec2{ it.sprite->region.extent.x, it.sprite->region.extent.y };
				vd[3].uv = it.sprite->region.pos + glm::vec2{ it.sprite->region.extent.x, 0.0f };

				bufferInfo_.map[0] = static_cast<Sprite::Vertex*>(bufferInfo_.map[0]) + 4;
			}
			if (bufferInfo_.map[1]) {
				uint32_t *id = static_cast<uint32_t*>(bufferInfo_.map[1]);
				id[0] = bufferInfo_.count;
				id[1] = bufferInfo_.count + 1;
				id[2] = bufferInfo_.count + 2;
				id[3] = bufferInfo_.count + 2;
				id[4] = bufferInfo_.count + 3;
				id[5] = bufferInfo_.count;
				
				bufferInfo_.map[1] = static_cast<uint32_t*>(bufferInfo_.map[1]) + 6;
			}
			bufferInfo_.count += 4;
		}

		//unmap and reset buffers
		for (int i = 0; i < 2; i++) {
			if (bufferInfo_.map[i]) {
				bufferInfo_.size[i] = 0;
				bufferInfo_.map[i] = nullptr;
				bufferInfo_.storage.unmap(i);
			}
		}
		bufferInfo_.offset = 0;
		bufferInfo_.count = 0;
	
		sprites_.clear();
	}

}
