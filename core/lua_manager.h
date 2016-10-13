#pragma once

#include "system.h"

#include <string>

#include "script/luah.h"

namespace oak {

	class LuaManager : public System {
	public:
		LuaManager(Engine &engine, const std::string &path);

		void init() override;
		void destroy() override;
	private:
		lua_State *L_;
		std::string path_;
		
	};

}