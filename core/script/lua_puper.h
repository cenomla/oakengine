#pragma once

#include "util/puper.h"

struct lua_State;

namespace oak {

	class Entity;

	class LuaPuper : public util::Puper {
	public:
		LuaPuper(lua_State *L, int index);

		void pup(int8_t &data, const util::ObjInfo &info) override;
		void pup(int16_t &data, const util::ObjInfo &info) override;
		void pup(int32_t &data, const util::ObjInfo &info) override;
		void pup(int64_t &data, const util::ObjInfo &info) override;
		void pup(uint8_t &data, const util::ObjInfo &info) override;
		void pup(uint16_t &data, const util::ObjInfo &info) override;
		void pup(uint32_t &data, const util::ObjInfo &info) override;
		void pup(uint64_t &data, const util::ObjInfo &info) override;
		void pup(float &data, const util::ObjInfo &info) override;
		void pup(double &data, const util::ObjInfo &info) override;
		void pup(bool &data, const util::ObjInfo &info) override;
		void pup(Entity &data, const util::ObjInfo &info) override;

		inline void setIndex(int index) { index_ = index; }

	private:
		lua_State *L_;
		int index_;
	};

};