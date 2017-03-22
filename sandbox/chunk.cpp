#include "chunk.h"

#include <pup.h>

namespace oak {
	void pup(Puper& puper, Tile& tile, const ObjInfo& info) {

		pup(puper, tile.dx, info + ObjInfo{ "dx" });
		pup(puper, tile.dy, info + ObjInfo{ "dy" });
		pup(puper, tile.dw, info + ObjInfo{ "dw" });
		pup(puper, tile.dh, info + ObjInfo{ "dh" });

		pup(puper, tile.offsetX, info + ObjInfo{ "offset_x" });
		pup(puper, tile.offsetY, info + ObjInfo{ "offset_y" });
		pup(puper, tile.width, info + ObjInfo{ "width" });
		pup(puper, tile.height, info + ObjInfo{ "height" });

		pup(puper, tile.animFrameX, info + ObjInfo{ "animframe_x" });
		pup(puper, tile.animFrameY, info + ObjInfo{ "animframe_y" });

		pup(puper, tile.material, info + ObjInfo{"material"});
		pup(puper, tile.flags, info + ObjInfo{"flags"});
	}

	void pup(Puper& puper, Chunk& chunk, const ObjInfo& info) {
		for (size_t i = 0; i < 256; i++) {
			pup(puper, chunk.tiles_[i], info + ObjInfo{ std::to_string(i).c_str() });
		}
		pup(puper, chunk.materialId_, info + ObjInfo{ "material_id" });
		pup(puper, chunk.tileCount_, info + ObjInfo{ "tile_count" });
		pup(puper, chunk.position_, info + ObjInfo{ "position" });
		pup(puper, chunk.extent_, info + ObjInfo{ "extent" });
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