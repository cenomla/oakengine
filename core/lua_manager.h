#pragma once

#include "oak_assert.h"

struct lua_State;

namespace oak {
	
	class LuaManager {
	private:
		static LuaManager *instance;
	public:
		static LuaManager& inst() {
			oak_assert(instance != nullptr);
			return *instance;
		}

		LuaManager();
		~LuaManager();

		inline lua_State* getState() const { return L_; }

	private:
		lua_State *L_;
	};

}
