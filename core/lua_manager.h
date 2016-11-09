#pragma once

#include "system.h"

#include <string>

#include "script/luah.h"

namespace oak {

	class LuaManager : public System {
	public:
		LuaManager(Engine &engine);

		void init() override;
		void destroy() override;

		inline lua_State* getState() { return L_; }
	private:
		lua_State *L_;
		
	};

}