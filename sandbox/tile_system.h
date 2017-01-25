#pragma once

#include <vector>
#include <graphics/opengl/gl_vertex_array.h>
#include <graphics/opengl/gl_buffer.h>
#include <graphics/opengl/gl_material.h>
#include <system.h>

#include "chunk.h"

class TileSystem : public oak::System {
public:
	TileSystem(oak::Engine &engine, int numChunks, int tileSize);

	void update();

	Chunk& getChunk(int x, int y, uint32_t layer);
	const Chunk& getChunk(int x, int y, uint32_t layer) const;

	void createLayer(size_t materialId);
	void destroyLayer(uint32_t layer);
	void moveLayer(uint32_t layer, uint32_t newLayer);

	void setLayerMaterial(uint32_t layer, size_t material);

private:
	std::vector<std::vector<Chunk>> chunks_;
	std::vector<size_t> chunkLayers_;
	std::vector<uint32_t> layers_;
	int numChunks_, tileSize_;
};