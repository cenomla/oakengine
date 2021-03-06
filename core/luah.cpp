#include "luah.h"

#include <iostream>
#include <cstring>
#include <cctype>
#include <lua/lua.hpp>

#include "util/string_util.h"
#include "file_manager.h"
#include "system_manager.h"
#include "lua_system.h"
#include "oak_alloc.h"
#include "log.h"

namespace oak {

	int addSystem(lua_State *L) {
		if (lua_gettop(L) != 2) { return 0; }
		const oak::string name{ lua_tostring(L, 2) };
		lua_pop(L, 1);
		lua_getfield(L, LUA_REGISTRYINDEX, "_oak_systems_");
		lua_rotate(L, -2, 1);
		lua_setfield(L, -2, name.c_str());
		auto ptr = static_cast<LuaSystem*>(oak_allocator.allocate(sizeof(LuaSystem)));
		new (ptr) LuaSystem{ L, name };
		//TODO:: fix leakage
		SystemManager::inst().addSystem(ptr, std::hash<oak::string>{}(name));
		return 0;
	}

	void registerCFunctions(lua_State *L) {
		lua_newtable(L);
		lua_pushcfunction(L, addSystem);
		lua_setfield(L, -2, "add_system");
		lua_setglobal(L, "oak");
	}

	lua_State* luah::createState() {
		log_print_out("creating the lua state");
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);

		const char* luaFun0 = R"(function getKeys(t) 
				local s = {}
				for k, v in pairs(t) do
					table.insert(s, k)
				end
				return s
			end)";

		luaL_dostring(L, luaFun0);

		const char* luaFun1 = R"(function prefab_newindex(table, key, value)
			local mt = getmetatable(table)
			if mt[key] ~= nil then
				mt[key] = value
			else
				rawset(table, key, value)
			end
		end)";

		luaL_dostring(L, luaFun1);

		lua_newtable(L);
		lua_setfield(L, LUA_REGISTRYINDEX, "_oak_systems_");

		registerCFunctions(L);

		return L;
	}

	void luah::destroyState(lua_State *L) {
		log_print_out("closing the Lua state: %i", lua_gettop(L));
		lua_close(L);
	}

	void luah::setMetatable(lua_State *L, const oak::string& tableName) {
		luah::getMetatable(L, tableName);
		lua_setmetatable(L, -2);
	}

	void luah::getMetatable(lua_State *L, const oak::string& tableName) {
		int isNewTable = luaL_newmetatable(L, tableName.c_str());
		if (isNewTable != 0) {
			//if the table is a new table then set it's index value to itself 
			lua_pushstring(L, "__index");
			luaL_getmetatable(L, tableName.c_str());
			lua_settable(L, -3);
		}
	}

	void luah::addFunctionToTable(lua_State *L, int idx, const oak::string& funcName, lua_CFunction func) {
		lua_pushcfunction(L, func);
		lua_setfield(L, idx, funcName.c_str());
	}

	void luah::addFunctionToMetatable(lua_State *L, const oak::string& tableName, const oak::string& funcName, lua_CFunction func) {
		luah::getMetatable(L, tableName);
		//add the function to the metatable 
		lua_pushcfunction(L, func);
		lua_setfield(L, -2, funcName.c_str());

		lua_pop(L, 1);
	}

	void luah::loadScript(lua_State *L, const oak::string& path) {
		log_print_out("loading script: %s", path.c_str());
		auto resolvedPath = FileManager::inst().resolvePath(path);
		luaL_loadfile(L, resolvedPath.c_str());
		int err = lua_pcall(L, 0, LUA_MULTRET, 0);

		if (err != LUA_OK) {
			const char* errMsg = lua_tostring(L, -1);
			log_print_warn("lua error in script %s : %i, %s", path.c_str(), err, errMsg);
			lua_pop(L, 1);
		}
	}

	void luah::getKeys(lua_State *L, int idx, oak::vector<oak::string>& keys) {
		lua_pushvalue(L, idx);
		lua_getglobal(L, "getKeys");
		lua_rotate(L, -2, 1);
		lua_pcall(L, 1, 1, 0);
		lua_pushnil(L);
		while (lua_next(L, -2)) {
			if (lua_type(L, -1) == LUA_TSTRING) {
				keys.push_back({ lua_tostring(L, -1), keys.get_allocator() });
			}
			lua_pop(L, 1);
		}

		lua_pop(L, 1);
	}

	void luah::getField(lua_State *L, int idx, const oak::string& field) {
		const oak::string delimeters{ ".[]", oak_allocator };

		lua_pushvalue(L, idx);

		int i = 0;
		size_t prev = 0, pos;
		do {
			pos = field.find_first_of(delimeters, prev);
			if (pos > prev) {
				const auto& t = field.substr(prev, pos-prev);
				if (strspn(t.c_str(), "0123456789") == t.size()) {
					lua_geti(L, -1, atoi(t.c_str()));
				} else {
					lua_getfield(L, -1, t.c_str());
				}
				i++;
				if (isNil(L, -1)) {
					break;
				}
			}
			prev = pos + 1;
		} while(pos != oak::string::npos);
		
		lua_rotate(L, -(i+1), 1);
		lua_pop(L, i);
	}

	void luah::setField(lua_State *L, int idx, const oak::string& field) {
		oak::string delimeters{ ".[]", oak_allocator };
		
		lua_pushvalue(L, idx);

		int i = 0;
		size_t prev = 0, pos;
		do {
			pos = field.find_first_of(delimeters, prev);
			if (pos > prev) {
				const auto& t = field.substr(prev, pos-prev);
				if (strspn(t.c_str(), "0123456789") == t.size()) {
					lua_geti(L, -1, atoi(t.c_str()));
				} else {
					lua_getfield(L, -1, t.c_str());
				}
				i++;

				if (isNil(L, -1)) {
					lua_pop(L, 1);
					lua_newtable(L);
					if (strspn(t.c_str(), "0123456789") == t.size()) {
						lua_seti(L, -2, atoi(t.c_str()));
					} else {
						lua_setfield(L, -2, t.c_str());
					}
					if (strspn(t.c_str(), "0123456789") == t.size()) {
						lua_geti(L, -1, atoi(t.c_str()));
					} else {
						lua_getfield(L, -1, t.c_str());
					}

				}
			}
			if (pos == oak::string::npos) {
				lua_pop(L, 1);
				const auto& t = field.substr(prev, pos-prev);
				lua_rotate(L, -(i + 1), -1);
				if (strspn(t.c_str(), "0123456789") == t.size()) {
					lua_seti(L, -2, atoi(t.c_str()));
				} else {
					lua_setfield(L, -2, t.c_str());
				}
				lua_pop(L, i);
			}
			prev = pos + 1;
		} while(pos != oak::string::npos);
		
	}

	void luah::getGlobal(lua_State *L, const oak::string& field) {
		lua_geti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		getField(L, -1, field);
		lua_rotate(L, -2, -1);
		lua_pop(L, 1);
	}

	void luah::setGlobal(lua_State *L, const oak::string& field) {
		lua_geti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		lua_rotate(L, -2, -1);
		setField(L, -2, field);
		lua_pop(L, 1);
	}

	void luah::call(lua_State *L, int nargs, int nreturns) {
		int err = lua_pcall(L, nargs, nreturns, 0);

		if (err != LUA_OK) {
			const char* errMsg = lua_tostring(L, -1);
			log_print_warn("lua error: %i, %s", err, errMsg);
			lua_pop(L, 1);
		}
	}

	void luah::pushValue(lua_State *L, int32_t v) {
		lua_pushinteger(L, v);
	}

	void luah::pushValue(lua_State *L, int64_t v) {
		lua_pushinteger(L, v);
	}

	void luah::pushValue(lua_State *L, uint32_t v) {
		lua_pushinteger(L, v);
	}

	void luah::pushValue(lua_State *L, uint64_t v) {
		lua_pushinteger(L, v);
	}

	void luah::pushValue(lua_State *L, float v) {
		lua_pushnumber(L, v);
	}

	void luah::pushValue(lua_State *L, double v) {
		lua_pushnumber(L, v);
	}

	void luah::pushValue(lua_State *L, void *v) {
		lua_pushlightuserdata(L, v);
	}

	void luah::pushValue(lua_State *L, const oak::string &v) {
		lua_pushstring(L, v.c_str());
	}

	template<> int32_t luah::toValue(lua_State *L, int idx) {
		return static_cast<int32_t>(lua_tointeger(L, idx));
	}

	template<> int64_t luah::toValue(lua_State *L, int idx) {
		return static_cast<int64_t>(lua_tointeger(L, idx));
	}

	template<> uint32_t luah::toValue(lua_State *L, int idx) {
		return static_cast<uint32_t>(lua_tointeger(L, idx));
	}

	template<> uint64_t luah::toValue(lua_State *L, int idx) {
		return static_cast<uint64_t>(lua_tointeger(L, idx));
	}

	template<> float luah::toValue(lua_State *L, int idx) {
		return static_cast<float>(lua_tonumber(L, idx));
	}

	template<> double luah::toValue(lua_State *L, int idx) {
		return static_cast<double>(lua_tonumber(L, idx));
	}

	template<> bool luah::toValue(lua_State *L, int idx) {
		return static_cast<bool>(lua_toboolean(L, idx));
	}

	template<> void* luah::toValue(lua_State *L, int idx) {
		return lua_touserdata(L, idx);
	}

	template<> oak::string luah::toValue(lua_State *L, int idx) {
		return oak::string{ lua_tostring(L, idx) };
	}

	bool luah::isNil(lua_State *L, int idx) {
		return lua_isnil(L, idx) == 0 ? false : true;
	}

}
