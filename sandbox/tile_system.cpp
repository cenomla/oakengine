#include "tile_system.h"

#include <system_manager.h>
#include <graphics/opengl/gl_renderer.h>

constexpr size_t INVALID_CHUNK = 0xFFFFFFFFFFFFFFFF;

TileSystem::TileSystem(int numChunks, int tileSize) : numChunks_{ numChunks }, tileSize_{ tileSize } {}

void TileSystem::run() {
	//auto &renderer = oak::SystemManager::inst().getSystem<oak::graphics::GLRenderer>();

	for (size_t i = 0; i < chunkLayers_.size(); i++) {
		const size_t cid = chunkLayers_[i];
		if (cid != INVALID_CHUNK) {
			for (const auto &chunk : chunks_[cid]) {
				//renderer.addObject(glm::vec2{ 0.0f }, depths_[i], layers_[i], 0.0f, 0.0f, &chunk);
			}
		}
	}
}

Chunk& TileSystem::getChunk(int x, int y, uint32_t layer) {
	return chunks_[chunkLayers_[layer]][(y / (tileSize_ * 16)) * numChunks_ + (x / (tileSize_ * 16))];
}

const Chunk& TileSystem::getChunk(int x, int y, uint32_t layer) const {
	return chunks_[chunkLayers_[layer]][(y / (tileSize_ * 16)) * numChunks_ + (x / (tileSize_ * 16))];
}

uint32_t TileSystem::createLayer(size_t materialId) {
	size_t id = chunks_.size();
	chunks_.push_back({});
	chunkLayers_.push_back(id);
	layers_.push_back(0);
	depths_.push_back(-1000000.0f);

	for (int i = 0; i < numChunks_ * numChunks_; i++) {
		Chunk chunk{ materialId, (i % numChunks_) * tileSize_ * 16, (i / numChunks_) * tileSize_ * 16, tileSize_, tileSize_ };
		for (int j = 0; j < 256; j++) {
			chunk.setTile(j % 16, j / 16, {});
		}
		chunks_[id].push_back(chunk);
	}

	return chunkLayers_.size() - 1;
}

void TileSystem::destroyLayer(uint32_t layer) {
	//swap and pop
	size_t cid = chunkLayers_[layer];
	chunkLayers_[layer] = INVALID_CHUNK;
	chunks_[cid] = std::move(chunks_[chunks_.size() - 1]);
	for (auto &l : chunkLayers_) {
		if (l == chunks_.size() - 1) {
			l = cid;
			break;
		}
	}
	for (size_t i = 0; i < chunkLayers_.size() - 1; i++) {
		if (i >= layer) {
			chunkLayers_[i] = chunkLayers_[i + 1];
		}
	}
	chunks_.pop_back();
	chunkLayers_.pop_back();
}

void TileSystem::moveLayer(uint32_t layer, uint32_t newLayer) {
	size_t cid = chunkLayers_[layer];
	int offset = layer < newLayer ? 1 : -1;
	for (size_t i = layer; i != newLayer; i += offset) {
		chunkLayers_[i] = chunkLayers_[i + offset];
	}

	chunkLayers_[newLayer] = cid;
}

void TileSystem::setLayerMaterial(uint32_t layer, size_t material) {
	if (layer < chunkLayers_.size() && chunkLayers_[layer] != INVALID_CHUNK) {
		for (auto &chunk : chunks_[chunkLayers_[layer]]) {
			chunk.setMaterialId(material);

		}
	}
}

void TileSystem::setLayerDepth(uint32_t layer, float depth) {
	if (layer < depths_.size()) {
		depths_[layer] = depth;
	}
}