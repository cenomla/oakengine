#include "lua_puper.h"

#include "script/luah.h"

namespace oak {

	LuaPuper::LuaPuper(lua_State *L, int index) : L_{ L }, index_{ index } {

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
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = luah::toValue<int32_t>(L_, -1);
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

	void LuaPuper::pup(int64_t &data, const util::ObjInfo &info) {
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = luah::toValue<int64_t>(L_, -1);
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
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
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = luah::toValue<uint32_t>(L_, -1);
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

	void LuaPuper::pup(uint64_t &data, const util::ObjInfo &info) {
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = luah::toValue<uint64_t>(L_, -1);
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

	void LuaPuper::pup(float &data, const util::ObjInfo &info) {
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = luah::toValue<float>(L_, -1);
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

	void LuaPuper::pup(double &data, const util::ObjInfo &info) {
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = luah::toValue<double>(L_, -1);
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

	void LuaPuper::pup(bool &data, const util::ObjInfo &info) {
		if (io_ == util::PuperIo::IN) {
			luah::getField(L_, index_, info.name);
			if (!luah::isNil(L_, -1)) {
				data = luah::toValue<bool>(L_, -1);
			}
			lua_pop(L_, 1);
		} else {
			luah::pushValue(L_, data);
			luah::setField(L_, index_, info.name);
		}
	}

}