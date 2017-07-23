#pragma once

#include <glm/glm.hpp>

#include "sprite.h"
#include "batch.h"
#include "material.h"

namespace oak::graphics {

	class BufferStorage;

	class SpriteBatcher {
	public:
		void setBufferStorage(BufferStorage *storage);
		void addSprite(uint32_t layer, const Material *material, const Sprite *sprite, const glm::mat3& transform);
		void run();
		
		inline const oak::vector<Batch>& getBatches() const { return batches_; }

	private:
		struct BufferInfo {
			BufferStorage *storage = nullptr;
			size_t size[2]{ 0 }, capacity[2]{ 0 };
			size_t offset = 0, count = 0;
			void *map[2]{ nullptr };
		} bufferInfo_;

		struct SpriteInfo {
			uint64_t layer;
			const Material *material;
			const Sprite *sprite;
			glm::mat3 transform;

			inline bool operator<(const SpriteInfo& rhs) const { 
				return layer == rhs.layer ? 
					material < rhs.material : 
					layer < rhs.layer; 
			}
		};

		oak::vector<SpriteInfo> sprites_;
		oak::vector<Batch> batches_;
	};

}