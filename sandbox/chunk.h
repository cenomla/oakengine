#pragma once

#include <glm/glm.hpp>
#include <graphics/renderable.h>

struct Tile {
	static constexpr uint64_t VISIBLE = 0x00000001;
	static constexpr uint64_t SOLID = 0x00000002;
	float dx = 0.0f, dy = 0.0f, dw = 0.0f, dh = 0.0f, offsetX = 0.0f, offsetY = 0.0f, width = 0.0f, height = 0.0f;
	int animFrameX = 0, animFrameY = 0;
	uint32_t material = 0;
	uint32_t flags = 0;

	struct Vertex {
		float x, y, dx, dy;
	};
};

namespace oak::util {
	class Puper;
	class ObjInfo;
}

void pup(oak::util::Puper &puper, Tile &tile, const oak::util::ObjInfo &info);

class Chunk : public oak::graphics::Renderable {
public:
	Chunk(size_t materialId, int x, int y, int width, int height);

	void draw(void *buffer, float x, float y, float rotation, float scale) const override;

	inline void setTile(int x, int y, const Tile& tile) { 
		Tile &t = tiles_[y * 16 + x];
		tileCount_ += (t.flags & Tile::VISIBLE) != (tile.flags & Tile::VISIBLE) ? 
			((tile.flags & Tile::VISIBLE) == Tile::VISIBLE ? 1 : -1) : 0;
		t = tile;
	}

	inline void setMaterialId(size_t id) { materialId_ = id; }

	inline const Tile& getTile(int x, int y) const { return tiles_[y * 16 + x]; }

	inline const glm::vec2& getPosition() const { return position_; }
	inline const glm::vec2& getExtent() const { return extent_; }

	inline size_t getMaterialId() const override { return materialId_; }
	inline size_t getVertexCount() const override { return tileCount_ * 4; }
private:
	Tile tiles_[256];
	size_t materialId_;
	size_t tileCount_;
	glm::vec2 position_;
	glm::vec2 extent_;
};