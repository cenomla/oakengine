#include "binding.h"

#include <script/luah.h>
#include <script/lua_puper.h>
#include <script/binding.h>
#include <engine.h>

#include "component_extentions.h"
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

//chunk get_chunk(x, y, layer)
static int c_getChunk(lua_State *L) {
	TileSystem& tileSystem = oak::Engine::inst().getSystem<TileSystem>();
	int x = toValue<int>(L, 1);
	int y = toValue<int>(L, 2);
	uint32_t layer = toValue<uint32_t>(L, 3);

	lua_settop(L, 0);

	auto &chunk = tileSystem.getChunk(x, y, layer);

	pushValue(L, chunk);

	return 1;
}

//uint32_t create_layer(matid)
static int c_createLayer(lua_State *L) {
	TileSystem &tileSystem = oak::Engine::inst().getSystem<TileSystem>();
	size_t matid = toValue<size_t>(L, 1);

	lua_settop(L, 0);

	uint32_t layer = tileSystem.createLayer(matid);

	pushValue(L, layer);

	return 1;
}

//void destroy_layer(matid)
static int c_destroyLayer(lua_State *L) {
	TileSystem &tileSystem = oak::Engine::inst().getSystem<TileSystem>();
	uint32_t layer = toValue<uint32_t>(L, 1);

	lua_settop(L, 0);

	tileSystem.destroyLayer(layer);

	return 0;
}

//void move_layer(layer, newlayer)
static int c_moveLayer(lua_State *L) {
	TileSystem &tileSystem = oak::Engine::inst().getSystem<TileSystem>();
	uint32_t layer = toValue<uint32_t>(L, 1);
	uint32_t newLayer = toValue<uint32_t>(L, 2);

	lua_settop(L, 0);

	tileSystem.moveLayer(layer, newLayer);

	return 0;
}

//void set_layer_dpeth(layer, depth)
static int c_setLayerDepth(lua_State *L) {
	TileSystem &tileSystem = oak::Engine::inst().getSystem<TileSystem>();
	uint32_t layer = toValue<uint32_t>(L, 1);
	float depth = toValue<float>(L, 2);

	lua_settop(L, 0);

	tileSystem.setLayerDepth(layer, depth);

	return 0;
}

void initBindings(lua_State *L) {
	addFunctionToMetatable(L, "entity", "add_light", oak::c_entity_addComponent<LightComponent>);
	addFunctionToMetatable(L, "entity", "remove_light", oak::c_entity_removeComponent<LightComponent>);
	addFunctionToMetatable(L, "entity", "get_light", oak::c_entity_getComponent<LightComponent>);
	addFunctionToMetatable(L, "entity", "set_light", oak::c_entity_setComponent<LightComponent>);
	addFunctionToMetatable(L, "entity", "has_light", oak::c_entity_hasComponent<LightComponent>);

	addFunctionToMetatable(L, "entity", "add_occluder", oak::c_entity_addComponent<OccluderComponent>);
	addFunctionToMetatable(L, "entity", "remove_occluder", oak::c_entity_removeComponent<OccluderComponent>);
	addFunctionToMetatable(L, "entity", "get_occluder", oak::c_entity_getComponent<OccluderComponent>);
	addFunctionToMetatable(L, "entity", "set_occluder", oak::c_entity_setComponent<OccluderComponent>);
	addFunctionToMetatable(L, "entity", "has_occluder", oak::c_entity_hasComponent<OccluderComponent>);

	addFunctionToMetatable(L, "chunk", "get_tile", c_getTile);
	addFunctionToMetatable(L, "chunk", "set_tile", c_setTile);

	addFunctionToMetatable(L, "tile_system", "get_chunk", c_getChunk);
	addFunctionToMetatable(L, "tile_system", "create_layer", c_createLayer);
	addFunctionToMetatable(L, "tile_system", "destroy_layer", c_destroyLayer);
	addFunctionToMetatable(L, "tile_system", "move_layer", c_moveLayer);
	addFunctionToMetatable(L, "tile_system", "set_layer_depth", c_setLayerDepth);

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

	lua_getglobal(L, "oak");
	lua_newtable(L);
	setMetatable(L, "tile_system");
	lua_setfield(L, -2, "ts");
	lua_pop(L, 1);
}

namespace oak::luah {

	void pushValue(lua_State *L, Chunk &chunk) {
		pushInstance(L, chunk);
		setMetatable(L, "chunk");
	}

	template<> Chunk& toValue(lua_State *L, int idx) {
		return toInstance<Chunk>(L, idx);
	}

}