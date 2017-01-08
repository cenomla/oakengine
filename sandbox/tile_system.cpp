#include "tile_system.h"

#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <graphics/opengl/gl_renderer.h>
#include <resource_manager.h>
#include <engine.h>

TileSystem::TileSystem(oak::Engine &engine, int numChunks, int tileSize) : 
	oak::System{ engine, "tile_system" }, numChunks_{ numChunks }, tileSize_{ tileSize } {

}

void TileSystem::init() {

	size_t matId = std::hash<std::string>{}("mat_tiles");

	//create chunks
	for (int i = 0; i < numChunks_ * numChunks_; i++) {
		Chunk chunk{ matId, (i % numChunks_) * tileSize_ * 16, (i / numChunks_) * tileSize_ * 16, tileSize_, tileSize_ };
		for (int j = 0; j < 256; j++) {
			chunk.setTile(j % 16, j / 16, { 0.0f, 0.0f, 0.0625f, 0.0625f, 0.0f, 0.0f, 
				static_cast<float>(tileSize_), static_cast<float>(tileSize_), 0, 0, 0, Tile::VISIBLE });
		}
		chunks_.push_back(chunk);
	}
}

void TileSystem::update() {

	auto &renderer = oak::Engine::inst().getSystem<oak::graphics::GLRenderer>();

	for (const auto &chunk : chunks_) {
		renderer.addObject(glm::vec2{ 0.0f }, -100000, 0, 0, 0, &chunk);
	}
}

Chunk& TileSystem::getChunk(int x, int y) {
	return chunks_[(y / (tileSize_ * 16)) * numChunks_ + (x / (tileSize_ * 16))];
}

const Chunk& TileSystem::getChunk(int x, int y) const {
	return chunks_[(y / (tileSize_ * 16)) * numChunks_ + (x / (tileSize_ * 16))];
}