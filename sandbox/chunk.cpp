#include "chunk.h"

#include <pup.h>

namespace oak {
	void pup(Puper &puper, Tile &tile, const ObjInfo &info) {

		pup(puper, tile.dx, ObjInfo{ "dx" } + info);
		pup(puper, tile.dy, ObjInfo{ "dy" } + info);
		pup(puper, tile.dw, ObjInfo{ "dw" } + info);
		pup(puper, tile.dh, ObjInfo{ "dh" } + info);

		pup(puper, tile.offsetX, ObjInfo{ "offset_x" } + info);
		pup(puper, tile.offsetY, ObjInfo{ "offset_y" } + info);
		pup(puper, tile.width, ObjInfo{ "width" } + info);
		pup(puper, tile.height, ObjInfo{ "height" } + info);

		pup(puper, tile.animFrameX, ObjInfo{ "animframe_x" } + info);
		pup(puper, tile.animFrameY, ObjInfo{ "animframe_y" } + info);

		pup(puper, tile.material, ObjInfo{"material"} + info);
		pup(puper, tile.flags, ObjInfo{"flags"} + info);
	}
}

Chunk::Chunk(size_t materialId, int x, int y, int width, int height) : materialId_{ materialId }, tileCount_{ 0 }, position_{ x, y }, extent_{ width, height } { }

void Chunk::draw(void *buffer, float x, float y, float rotation, float scale) const {
	Tile::Vertex *data = static_cast<Tile::Vertex*>(buffer);
	size_t offset = 0;

	for (size_t i = 0; i < 256; i++) {
		const Tile& tile = tiles_[i];
		if ((tile.flags & Tile::VISIBLE) == Tile::VISIBLE) {
			float tx = position_.x + ((i % 16) * extent_.x) + tile.offsetX;
			float ty = position_.y + ((i / 16) * extent_.y) + tile.offsetY;
			float dx = tile.dx + tile.animFrameX * tile.dw;
			float dy = tile.dy + tile.animFrameY * tile.dh;

			data[offset++] = Tile::Vertex{ tx, ty, dx, dy };
			data[offset++] = Tile::Vertex{ tx + tile.width, ty, dx + tile.dw, dy };
			data[offset++] = Tile::Vertex{ tx + tile.width, ty + tile.height, dx + tile.dw, dy + tile.dh };
			data[offset++] = Tile::Vertex{ tx, ty + tile.height, dx, dy + tile.dh };
		}
	}
}