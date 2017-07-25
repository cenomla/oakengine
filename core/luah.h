#pragma once

#include <lua/lua.hpp>

#include "container.h"

namespace oak {

	namespace luah {

		lua_State* createState();
		void destroyState(lua_State *L);

		//c++ object instance functions
		template <class T>
		T& toInstance(lua_State *L, int idx) {
			return *static_cast<T*>(*reinterpret_cast<void**>(lua_touserdata(L, idx)));
		}

		template <class T>
		void pushInstance(lua_State *L, T& i) {
			void** ptr = reinterpret_cast<void**>(lua_newuserdata(L, sizeof(void*)));
			*ptr = &i;
		}


		void pushValue(lua_State *L, int32_t v);
		void pushValue(lua_State *L, int64_t v);
		void pushValue(lua_State *L, uint32_t v);
		void pushValue(lua_State *L, uint64_t v);
		void pushValue(lua_State *L, float v);
		void pushValue(lua_State *L, double v);
		void pushValue(lua_State *L, const oak::string& v);

		template<typename T>
		T toValue(lua_State *L, int idx);
		
		template<> int32_t toValue(lua_State *L, int idx);
		template<> int64_t toValue(lua_State *L, int idx);
		template<> uint32_t toValue(lua_State *L, int idx);
		template<> uint64_t toValue(lua_State *L, int idx);
		template<> float toValue(lua_State *L, int idx);
		template<> double toValue(lua_State *L, int idx);
		template<> bool toValue(lua_State *L, int idx);
		template<> oak::string toValue(lua_State *L, int idx);

		bool isNil(lua_State *L, int idx);


		//metatable functions
		void setMetatable(lua_State *L, const oak::string& tableName);
		void getMetatable(lua_State *L, const oak::string& tableName);

		//function functions
		void addFunctionToTable(lua_State *L, int idx, const oak::string& funcName, lua_CFunction func);
		void addFunctionToMetatable(lua_State *L, const oak::string& tableName, const oak::string& funcName, lua_CFunction func);

		void loadScript(lua_State *L, const oak::string& path);
		void getKeys(lua_State *L, int idx, oak::vector<oak::string>& keys);

		void getField(lua_State *L, int idx, const oak::string& field);
		void setField(lua_State *L, int idx, const oak::string& field);
		void getGlobal(lua_State *L, const oak::string& field);
		void setGlobal(lua_State *L, const oak::string& field);

		void call(lua_State *L, int nargs, int nreturns);
		
	}

}