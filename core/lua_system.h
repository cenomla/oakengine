#pragma once

#include "system.h"
#include "luah.h"

namespace oak {

	class LuaManager : public System {
	public:

		void init() override;
		void terminate() override;

		lua_State* getState() { return L_; }
	private:
		lua_State *L_;
		
	};

}