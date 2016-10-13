#pragma once

#include <string>
#include <vector>
#include <lua/lua.hpp>

namespace oak {

	namespace luah {

		lua_State* createState();
		void destroyState(lua_State *L);

		//c++ object instance functions
		template <class T>
		T& toInstance(lua_State *L, int idx) {
			T& i = *static_cast<T*>(*reinterpret_cast<void**>(lua_touserdata(L, idx)));
			return i;
		}

		template <class T>
		T& getInstance(lua_State *L, int idx) {
			T& i = toInstance<T>(L, idx);
			lua_remove(L, idx);
			return i;
		}

		template <class T>
		void pushInstance(lua_State *L, const T &i) {
			const void** ptr = reinterpret_cast<const void**>(lua_newuserdata(L, sizeof(void*)));
			*ptr = &i;
		}

		void pushValue(lua_State *L);

		void pushValue(lua_State *L, int v);
		void pushValue(lua_State *L, size_t v);
		void pushValue(lua_State *L, float v);
		void pushValue(lua_State *L, double v);
		void pushValue(lua_State *L, const char* v);

		template<typename T>
		T toValue(lua_State *L, int idx);

		void pushTable(lua_State *L);


		//metatable functions
		void setMetatable(lua_State *L, const std::string &tableName);
		void getMetatable(lua_State *L, const std::string &tableName);
		void addFunctionToMetatable(lua_State *L, const std::string &tableName, const std::string &funcName, lua_CFunction func);

		void loadScript(lua_State *L, const std::string &path);
		std::vector<std::string> getKeys(lua_State *L);
		void getField(lua_State *L, const std::string &field);
		void call(lua_State *L, int nargs, int nreturns);
		
	}

}