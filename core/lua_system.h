#pragma once

#include "container.h"
#include "system.h"
#include "luah.h"

namespace oak {

	class LuaSystem : public System {
	public:
		LuaSystem(lua_State *L, const oak::string& name);

		void init() override;
		void terminate() override;

		void run() override;
	private:
		lua_State *L_;
		oak::string name_;
	};

}
