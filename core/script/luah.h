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
		void pushValue(lua_State *L, const std::string& v);
		void pushValue(lua_State *L, void* v);

		template<typename T>
		T toValue(lua_State *L, int idx);
		
		template<> int32_t toValue(lua_State *L, int idx);
		template<> int64_t toValue(lua_State *L, int idx);
		template<> uint32_t toValue(lua_State *L, int idx);
		template<> uint64_t toValue(lua_State *L, int idx);
		template<> float toValue(lua_State *L, int idx);
		template<> double toValue(lua_State *L, int idx);
		template<> bool toValue(lua_State *L, int idx);
		template<> std::string toValue(lua_State *L, int idx);

		bool isNil(lua_State *L, int idx);


		//metatable functions
		void setMetatable(lua_State *L, const std::string& tableName);
		void getMetatable(lua_State *L, const std::string& tableName);

		//function functions
		void addFunctionToTable(lua_State *L, int idx, const std::string& funcName, lua_CFunction func);
		void addFunctionToMetatable(lua_State *L, const std::string& tableName, const std::string& funcName, lua_CFunction func);

		void loadScript(lua_State *L, const std::string& path);
		std::vector<std::string> getKeys(lua_State *L);
		void getGlobal(lua_State *L, const std::string& field);
		void getRegistry(lua_State *L, const std::string& field);
		void getField(lua_State *L, int idx, const std::string& field);
		void setField(lua_State *L, int idx, const std::string& field);
		void call(lua_State *L, int nargs, int nreturns);
		
	}

}