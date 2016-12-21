#include "tile_system.h"

#include <glm/gtc/matrix_transform.hpp>

TileSystem::TileSystem(oak::Engine &engine, int numChunks, int tileSize) : 
	oak::System{ engine, "tile_system" }, numChunks_{ numChunks }, tileSize_{ tileSize }, vbo_{ GL_ARRAY_BUFFER } {

}

void TileSystem::init() {

	//create chunks
	for (int i = 0; i < numChunks_ * numChunks_; i++) {
		Chunk chunk{ (i % numChunks_) * tileSize_ * 16, (i / numChunks_) * tileSize_ * 16, tileSize_, tileSize_ };
		for (int j = 0; j < 256; j++) {
			chunk.setTile(j % 16, j / 16, { 0.0f, 0.0f, 0.0625f, 0.0625f, 0.0f, 0.0f, 
				static_cast<float>(tileSize_), static_cast<float>(tileSize_), 0, 0, 0, Tile::VISIBLE });
		}
		chunks_.push_back(chunk);
	}

	//setup graphics

	vao_.create();
	vao_.bind();

	vbo_.create();
	vbo_.bind();
	vbo_.bufferData(1572864, nullptr, GL_STREAM_DRAW);
	vao_.attributeDescription(
			{0, 0, sizeof(Tile::Vertex)}, {
				{0, 0, 2, offsetof(Tile::Vertex, Tile::Vertex::x) },
				{0, 1, 2, offsetof(Tile::Vertex, Tile::Vertex::dx) }
			});
	vbo_.unbind();
	vao_.unbind();
}

void TileSystem::render(const oak::graphics::GLMaterial &mat) {
	vbo_.bind();
	float *buffer = static_cast<float*>(vbo_.map(GL_WRITE_ONLY));

	for (const auto &chunk : chunks_) {
		buffer += chunk.draw(buffer) * sizeof(Tile::Vertex) * 6 / 4;
	}

	vbo_.unmap();
	vbo_.unbind();

	vao_.bind();
	mat.bind();
	glDrawArrays(GL_TRIANGLES, 0, 98304);
	vao_.unbind();
}

Chunk& TileSystem::getChunk(int x, int y) {
	return chunks_[(y / (tileSize_ * 16)) * numChunks_ + (x / (tileSize_ * 16))];
}

const Chunk& TileSystem::getChunk(int x, int y) const {
	return chunks_[(y / (tileSize_ * 16)) * numChunks_ + (x / (tileSize_ * 16))];
}