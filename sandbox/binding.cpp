#include "binding.h"

#include <script/luah.h>
#include <script/lua_puper.h>
#include <engine.h>

#include "tile_system.h"
#include "events.h"

using namespace oak::luah;


//tile getTile(chunk, x, y)
static int c_getTile(lua_State *L) {
	Chunk& chunk = toValue<Chunk&>(L, 1);
	int x = toValue<int>(L, 2);
	int y = toValue<int>(L, 3);
	lua_settop(L, 0);

	auto tile = chunk.getTile(x, y);

	lua_newtable(L);
	oak::LuaPuper puper{ L, 1 };
	pup(puper, tile, {});

	return 1;
}

//void setTile(chunk, x, y, tile)
static int c_setTile(lua_State *L) {
	Chunk &chunk = toValue<Chunk&>(L, 1);
	int x = toValue<int>(L, 2);
	int y = toValue<int>(L, 3);

	auto tile = chunk.getTile(x, y);

	oak::LuaPuper puper{ L, 4 };
	puper.setIo(oak::PuperIo::IN);
	pup(puper, tile, {});

	chunk.setTile(x, y, tile);

	lua_settop(L, 0);

	return 0;
}

//chunk get_chunk(tileSystem, x, y, layer)
static int c_getChunk(lua_State *L) {
	TileSystem& tileSystem = toValue<TileSystem&>(L, 1);
	int x = toValue<int>(L, 2);
	int y = toValue<int>(L, 3);
	uint32_t layer = toValue<uint32_t>(L, 4);

	lua_settop(L, 0);

	auto &chunk = tileSystem.getChunk(x, y, layer);

	pushValue(L, chunk);

	return 1;
}

void initBindings(lua_State *L) {
	addFunctionToMetatable(L, "chunk", "get_tile", c_getTile);
	addFunctionToMetatable(L, "chunk", "set_tile", c_setTile);

	addFunctionToMetatable(L, "tile_system", "get_chunk", c_getChunk);

	auto &engine = oak::Engine::inst();

	engine.getEventManager().add<TileCollisionEvent>([L](TileCollisionEvent evt) {
		oak::luah::getGlobal(L, "oak.es.send_event");
		oak::luah::getGlobal(L, "oak.es");
		oak::luah::pushValue(L, "on_tile_collide");
		
		lua_newtable(L);
		oak::LuaPuper puper{ L, -2 };
		pup(puper, evt, {});

		oak::luah::call(L, 3, 0);
	});
	//add the tile system to the oak global table
	lua_getglobal(L, "oak");
	oak::luah::pushValue(L, engine.getSystem<TileSystem>());
	lua_setfield(L, -2, "ts");
	lua_pop(L, 1);
}

namespace oak::luah {

	void pushValue(lua_State *L, TileSystem &tileSystem) {
		pushInstance(L, tileSystem);
		setMetatable(L, "tile_system");
	}

	template<> TileSystem& toValue(lua_State *L, int idx) {
		return toInstance<TileSystem>(L, idx);
	}

	void pushValue(lua_State *L, Chunk &chunk) {
		pushInstance(L, chunk);
		setMetatable(L, "chunk");
	}

	template<> Chunk& toValue(lua_State *L, int idx) {
		return toInstance<Chunk>(L, idx);
	}

}