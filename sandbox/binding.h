#pragma once

#include <script/luah.h>

class Chunk;

void initBindings(lua_State *L);

namespace oak::luah {

	void pushValue(lua_State *L, Chunk &chunk);
	template<> Chunk& toValue(lua_State *L, int idx);
}