#include "tile_system.h"

#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/opengl/gl_renderer.h>
#include <resource_manager.h>
#include <engine.h>

constexpr size_t INVALID_CHUNK = 0xFFFFFFFFFFFFFFFF;

TileSystem::TileSystem(oak::Engine &engine, int numChunks, int tileSize) : 
	oak::System{ engine, "tile_system" }, numChunks_{ numChunks }, tileSize_{ tileSize } {

}

void TileSystem::update() {
	auto &renderer = oak::Engine::inst().getSystem<oak::graphics::GLRenderer>();

	for (size_t i = 0; i < chunkLayers_.size(); i++) {
		const size_t cid = chunkLayers_[i];
		if (cid != INVALID_CHUNK) {
			for (const auto &chunk : chunks_[cid]) {
				renderer.addObject(glm::vec2{ 0.0f }, -1000000.0f, layers_[i], 0.0f, 0.0f, &chunk);
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

void TileSystem::createLayer(size_t materialId) {
	size_t id = chunks_.size();
	chunks_.push_back({});
	chunkLayers_.push_back(id);
	layers_.push_back(0);

	for (int i = 0; i < numChunks_ * numChunks_; i++) {
		Chunk chunk{ materialId, (i % numChunks_) * tileSize_ * 16, (i / numChunks_) * tileSize_ * 16, tileSize_, tileSize_ };
		for (int j = 0; j < 256; j++) {
			chunk.setTile(j % 16, j / 16, {});
		}
		chunks_[id].push_back(chunk);
	}
}

void TileSystem::destroyLayer(uint32_t layer) {
	//swap and pop
	chunks_[chunkLayers_[layer]] = std::move(chunks_[chunks_.size() - 1]);
	for (auto &l : chunkLayers_) {
		if (l == chunks_.size() - 1) {
			l = chunkLayers_[layer];
			chunkLayers_[layer] = INVALID_CHUNK;
			break;
		}
	}
	chunks_.pop_back();
}

void TileSystem::moveLayer(uint32_t layer, uint32_t newLayer) {
	size_t cid = chunkLayers_[newLayer];
	for (auto &l : chunkLayers_) {
		if (l == chunks_.size() - 1) {
			l--;
			break;
		}
	}
	for (auto &l : chunkLayers_) {
		if (l >= cid) {
			l++;
		}
	}
	chunkLayers_[layer] = cid;
}

void TileSystem::setLayerMaterial(uint32_t layer, size_t material) {
	if (layer < chunkLayers_.size() && chunkLayers_[layer] != INVALID_CHUNK) {
		for (auto &chunk : chunks_[chunkLayers_[layer]]) {
			chunk.setMaterialId(material);
		}
	}
}