#include "lua_puper.h"

#include "script/luah.h"

namespace oak {

	LuaPuper::LuaPuper(lua_State *L, int index) : L_{ L }, index_{ index } {

	} 

	template<class T>
	void lua_pup(PuperIo io, lua_State *L, int index, T &data, const ObjInfo &info) {
		if (io == PuperIo::IN) {
			int p = 1;
			luah::getField(L, index, info.name);
			if (!luah::isNil(L, -1)) {
				if ((info.flags & ObjInfo::SIZE_VAR) == ObjInfo::SIZE_VAR) {
					lua_len(L, -1);
					p = 2;
				}
				data = luah::toValue<T>(L, -1);
			} else if ((info.flags & ObjInfo::SIZE_VAR) == ObjInfo::SIZE_VAR) {
				lua_len(L, index);
				p = 2;
				data = luah::toValue<T>(L, -1);
			}
			lua_pop(L, p);
		} else {
			luah::pushValue(L, data);
			luah::setField(L, index, info.name);
		}
	}

	void LuaPuper::pup(int8_t &data, const ObjInfo &info) {
		int32_t var = data;
		lua_pup<int32_t>(io_, L_, index_, var, info);
		data = static_cast<int8_t>(var);
	}

	void LuaPuper::pup(int16_t &data, const ObjInfo &info) {
		int32_t var = data;
		lua_pup<int32_t>(io_, L_, index_, var, info);
		data = static_cast<int16_t>(var);
	}

	void LuaPuper::pup(int32_t &data, const ObjInfo &info) {
		lua_pup<int32_t>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(int64_t &data, const ObjInfo &info) {
		lua_pup<int64_t>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(uint8_t &data, const ObjInfo &info) {
		uint32_t var = data;
		lua_pup<uint32_t>(io_, L_, index_, var, info);
		data = static_cast<uint8_t>(var);
	}

	void LuaPuper::pup(uint16_t &data, const ObjInfo &info) {
		uint32_t var = data;
		lua_pup<uint32_t>(io_, L_, index_, var, info);
		data = static_cast<uint16_t>(var);
	}

	void LuaPuper::pup(uint32_t &data, const ObjInfo &info) {
		lua_pup<uint32_t>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(uint64_t &data, const ObjInfo &info) {
		lua_pup<uint64_t>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(float &data, const ObjInfo &info) {
		lua_pup<float>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(double &data, const ObjInfo &info) {
		lua_pup<double>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(bool &data, const ObjInfo &info) {
		lua_pup<bool>(io_, L_, index_, data, info);
	}

	void LuaPuper::pup(oak::string &data, const ObjInfo &info) {
		lua_pup<oak::string>(io_, L_, index_, data, info);
	}

}