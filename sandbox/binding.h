#pragma once

#include <script/luah.h>

class TileSystem;
class Chunk;

void initBindings(lua_State *L);

namespace oak::luah {
	void pushValue(lua_State *L, TileSystem &tileSystem);
	template<> TileSystem& toValue(lua_State *L, int idx);

	void pushValue(lua_State *L, Chunk &chunk);
	template<> Chunk& toValue(lua_State *L, int idx);
}