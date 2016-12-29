#include "lua_puper.h"

#include "script/luah.h"
#include "binding.h"
#include "entity.h"

namespace oak {

	LuaPuper::LuaPuper(lua_State *L, int index) : L_{ L }, index_{ index } {

	} 

	template<class T>
	void lua_pup(util::PuperIo io, lua_State *L, int index, T &data, const util::ObjInfo &info) {
		if (io == util::PuperIo::IN) {
			luah::getField(L, index, info.name);
			if (!luah::isNil(L, -1)) {
				data = luah::toValue<T>(L, -1);
			}
			lua_pop(L, 1);
		} else {
			luah::pushValue(L, data);
			luah::setField(L, index, info.name);
		}
	}

	void LuaPuper::pup(int8_t &data, const util::ObjInfo &info) {
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = static_cast<int8_t>(luah::toValue<int32_t>(L_, -1));
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

	void LuaPuper::pup(int16_t &data, const util::ObjInfo &info) {
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = static_cast<int16_t>(luah::toValue<int32_t>(L_, -1));
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

	void LuaPuper::pup(int32_t &data, const util::ObjInfo &info) {
		lua_pup<int32_t>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(int64_t &data, const util::ObjInfo &info) {
		lua_pup<int64_t>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(uint8_t &data, const util::ObjInfo &info) {
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = static_cast<uint8_t>(luah::toValue<uint32_t>(L_, -1));
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

	void LuaPuper::pup(uint16_t &data, const util::ObjInfo &info) {
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = static_cast<uint16_t>(luah::toValue<uint32_t>(L_, -1));
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

	void LuaPuper::pup(uint32_t &data, const util::ObjInfo &info) {
		lua_pup<uint32_t>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(uint64_t &data, const util::ObjInfo &info) {
		lua_pup<uint64_t>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(float &data, const util::ObjInfo &info) {
		lua_pup<float>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(double &data, const util::ObjInfo &info) {
		lua_pup<double>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(bool &data, const util::ObjInfo &info) {
		lua_pup<bool>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(std::string &data, const util::ObjInfo &info) {
		lua_pup<std::string>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(Entity &data, const util::ObjInfo &info) {
		lua_pup<Entity>(io_, L_, index_, data, info);
	}

}