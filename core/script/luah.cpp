#include "luah.h"

#include <iostream>

#include "log.h"

namespace oak {

	lua_State* luah::createState() {
		log::cout << "creating the lua state" << std::endl;
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);

		static const std::string luaFun = R"(function getKeys(t) 
				local s = {}
				for k, v in pairs(t) do
					table.insert(s, k)
				end
				return s
			end)";

		luaL_dostring(L, luaFun.c_str());

		return L;
	}

	void luah::destroyState(lua_State *L) {
		log::cout << "closing the Lua state : " << lua_gettop(L) << std::endl;
		lua_close(L);
	}

	void luah::setMetatable(lua_State *L, const std::string &tableName) {
		luah::getMetatable(L, tableName);
		lua_setmetatable(L, -2);
	}

	void luah::getMetatable(lua_State *L, const std::string &tableName) {
		int isNewTable = luaL_newmetatable(L, tableName.c_str());
		if (isNewTable != 0) {
			//if the table is a new table then set it's index value to itself 
			lua_pushstring(L, "__index");
			luaL_getmetatable(L, tableName.c_str());
			lua_settable(L, -3);
		}
	}

	void luah::addFunctionToMetatable(lua_State *L, const std::string &tableName, const std::string &funcName, lua_CFunction func) {
		luah::getMetatable(L, tableName);
		//add the function to the metatable
		lua_pushstring(L, funcName.c_str()); 
		lua_pushcfunction(L, func);
		lua_settable(L, -3);

		lua_pop(L, 1);
	}

	void luah::loadScript(lua_State *L, const std::string &path) {
		log::cout << "loading script: " << path << std::endl;
		luaL_loadfile(L, path.c_str());
		int err = lua_pcall(L, 0, LUA_MULTRET, 0);

		if (err != LUA_OK) {
			std::string errMsg = lua_tostring(L, -1);
			log::cout << "lua error in script " << path << " : " << err << ", " << errMsg << std::endl;
			lua_pop(L, 1);
		}
	}

	std::vector<std::string> luah::getKeys(lua_State *L) {
		std::vector<std::string> keys;
		//L: table
		lua_getglobal(L, "getKeys");
		lua_rotate(L, -2, 1);
		//L: getKeys() entityTable
		lua_pcall(L, 1, 1, 0);
		lua_pushnil(L);
		//L: table nil
		while (lua_next(L, -2)) {
			if (lua_type(L, -1) == LUA_TSTRING) {
				keys.push_back(lua_tostring(L, -1));
			}
			lua_pop(L, 1);
		}

		lua_pop(L, 1);

		return keys;
	}

	void luah::getGlobal(lua_State *L, const std::string &table) {
		size_t pos = 0, len = 0, count = 0;

		while ((len = table.find('.', pos)) != std::string::npos) {
			if (count == 0) {
				lua_getglobal(L, table.substr(pos, len - pos).c_str());
			} else {
				lua_getfield(L, -1, table.substr(pos, len - pos).c_str());
			}

			pos = len + 1;
			count++;

			//if we encounter a null field leave it on the stack and return
			if (isNil(L, -1)) {
				lua_rotate(L, -count, 1);
				lua_pop(L, count - 1);
				return;
			}
		}
		if (count == 0) {
			lua_getglobal(L, table.c_str());
		} else {
			lua_getfield(L, -1, table.substr(pos, table.size()).c_str());
			lua_rotate(L, -(count + 1), 1);
			lua_pop(L, count);
		}
	}

	void luah::getRegistry(lua_State *L, const std::string &table) {
		size_t pos = 0, len = 0, count = 0;

		while ((len = table.find('.', pos)) != std::string::npos) {
			if (count == 0) {
				lua_getfield(L, LUA_REGISTRYINDEX, table.substr(pos, len - pos).c_str());
			} else {
				lua_getfield(L, -1, table.substr(pos, len - pos).c_str());
			}

			pos = len + 1;
			count++;

			//if we encounter a null field leave it on the stack and return
			if (isNil(L, -1)) {
				lua_rotate(L, -count, 1);
				lua_pop(L, count - 1);
				return;
			}
		}
		if (count == 0) {
			lua_getfield(L, LUA_REGISTRYINDEX, table.c_str());
		} else {
			lua_getfield(L, -1, table.substr(pos, table.size()).c_str());
			lua_rotate(L, -(count + 1), 1);
			lua_pop(L, count);
		}
	}

	void luah::getField(lua_State *L, int idx, const std::string &table) {
		size_t pos = 0, len = 0, count = 0;

		lua_pushvalue(L, idx);

		while ((len = table.find('.', pos)) != std::string::npos) {
			lua_getfield(L, -1, table.substr(pos, len - pos).c_str());

			pos = len + 1;
			count++;

			//if we encounter a null field leave it on the stack and return
			if (isNil(L, -1)) {
				lua_rotate(L, -(count + 1), 1);
				lua_pop(L, count);
				return;
			}
		}

		lua_getfield(L, -1, table.substr(pos, table.size()).c_str());
		lua_rotate(L, -(count + 2), 1);
		lua_pop(L, count + 1);
	}

	void luah::setField(lua_State *L, int idx, const std::string &table) {
		size_t pos = 0, len = 0, count = 0;

		lua_pushvalue(L, idx);

		while ((len = table.find('.', pos)) != std::string::npos) {
			const std::string &token = table.substr(pos, len - pos);

			lua_getfield(L, -1, token.c_str());
			if (isNil(L, -1)) {
				lua_pop(L, 1);
				lua_newtable(L);
				lua_setfield(L, -2, token.c_str());
				lua_getfield(L, -1, token.c_str());
			}

			pos = len + 1;
			count++;
		}
		const std::string &token = table.substr(pos, table.size());
		lua_rotate(L, -(count + 2), -1);
		lua_setfield(L, -2, token.c_str());
		lua_pop(L, count + 1);
	}

	void luah::call(lua_State *L, int nargs, int nreturns) {
		int err = lua_pcall(L, nargs, nreturns, 0);

		if (err != LUA_OK) {
			std::string errMsg = lua_tostring(L, -1);
			log::cout << "lua error: " << err << ", " << errMsg << std::endl;
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

	void luah::pushValue(lua_State *L, const std::string &v) {
		lua_pushstring(L, v.c_str());
	}

	void luah::pushValue(lua_State *L, void* v) {
		lua_pushlightuserdata(L, v);
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

	template<> std::string luah::toValue(lua_State *L, int idx) {
		return std::string{ lua_tostring(L, idx) };
	}

	bool luah::isNil(lua_State *L, int idx) {
		return lua_isnil(L, idx) == 0 ? false : true;
	}

}