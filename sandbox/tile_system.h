#pragma once

#include <container.h>
#include <system.h>

#include "chunk.h"

class TileSystem : public oak::System {
public:
	TileSystem(int numChunks, int tileSize);

	void run() override;

	Chunk& getChunk(int x, int y, uint32_t layer);
	const Chunk& getChunk(int x, int y, uint32_t layer) const;

	uint32_t createLayer(size_t materialId);
	void destroyLayer(uint32_t layer);
	void moveLayer(uint32_t layer, uint32_t newLayer);

	void setLayerMaterial(uint32_t layer, size_t material);
	void setLayerDepth(uint32_t layer, float depth);

private:
	oak::vector<oak::vector<Chunk>> chunks_;
	oak::vector<size_t> chunkLayers_;
	oak::vector<uint32_t> layers_;
	oak::vector<float> depths_;
	int numChunks_, tileSize_;
};