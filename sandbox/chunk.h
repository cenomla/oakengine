#pragma once

#include <glm/glm.hpp>

struct Tile {
	static constexpr uint64_t VISIBLE = 0x00000001;
	static constexpr uint64_t SOLID = 0x00000002;
	float dx, dy, dw, dh, offsetX, offsetY, width, height;
	int animFrameX, animFrameY;
	uint64_t flags;

	struct Vertex {
		float x, y, dx, dy;
	};
};

namespace oak::util {
	class Puper;
	class ObjInfo;
}

void pup(oak::util::Puper &puper, Tile &tile, const oak::util::ObjInfo &info);

class Chunk {
public:
	Chunk(int x, int y, int width, int height);

	void draw(void *buffer) const;

	inline void setTile(int x, int y, const Tile& tile) { tiles_[y * 16 + x] = tile; }
	inline const Tile& getTile(int x, int y) const { return tiles_[y * 16 + x]; }
	inline Tile& getTile(int x, int y) { return tiles_[y * 16 + x]; }

	inline const glm::vec2& getPosition() const { return position_; }
	inline const glm::vec2& getExtent() const { return extent_; }
private:
	Tile tiles_[256];
	glm::vec2 position_;
	glm::vec2 extent_;
};