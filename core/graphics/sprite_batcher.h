#pragma once

#include "math.h"
#include "buffer_storage.h"
#include "sprite.h"
#include "batch.h"
#include "material.h"

namespace oak::graphics {

	class BufferStorage;

	class SpriteBatcher {
	public:
		void init();
		void terminate();

		void addSprite(uint32_t layer, const Material *material, const Sprite *sprite, const Mat3& transform);
		void run();
		
		inline const oak::vector<Batch>& getBatches() const { return batches_; }

	private:
		struct BufferInfo {
			BufferStorage storage;
			size_t size[2]{ 0 }, capacity[2]{ 0 };
			size_t offset = 0, count = 0;
			void *map[2]{ nullptr };
		} bufferInfo_;

		struct SpriteInfo {
			uint64_t layer;
			const Material *material;
			const Sprite *sprite;
			Mat3 transform;

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
