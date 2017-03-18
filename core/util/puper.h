#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "container.h"

namespace oak {

	struct ObjInfo {
		constexpr static uint32_t SIZE_VAR = 0x00000001;

		oak::string name;
		uint32_t flags = 0;

		inline const ObjInfo& combine(const ObjInfo &other) {
			if (!name.empty()) {
				if (!other.name.empty()) {
					name = other.name + "." + name;
				}
			} else {
				name = other.name;
			}
			flags |= other.flags;
			
			return *this;
		}
	};

	inline const ObjInfo& operator+(const ObjInfo& lhs, ObjInfo&& rhs) {
		return rhs.combine(lhs);
	}

	enum class PuperIo {
		IN,
		OUT
	};

	class Entity;

	class Puper {
	public:
		virtual ~Puper();

		virtual void pup(int8_t &data, const ObjInfo &info) = 0;
		virtual void pup(int16_t &data, const ObjInfo &info) = 0;
		virtual void pup(int32_t &data, const ObjInfo &info) = 0;
		virtual void pup(int64_t &data, const ObjInfo &info) = 0;
		virtual void pup(uint8_t &data, const ObjInfo &info) = 0;
		virtual void pup(uint16_t &data, const ObjInfo &info) = 0;
		virtual void pup(uint32_t &data, const ObjInfo &info) = 0;
		virtual void pup(uint64_t &data, const ObjInfo &info) = 0;
		virtual void pup(float &data, const ObjInfo &info) = 0;
		virtual void pup(double &data, const ObjInfo &info) = 0;
		virtual void pup(bool &data, const ObjInfo &info) = 0;
		virtual void pup(oak::string &data, const ObjInfo &info) = 0;
		virtual void pup(Entity &data, const ObjInfo &info) = 0;
	
		inline void setIo(PuperIo io) { io_ = io; }
		inline PuperIo getIo() const { return io_; }
	protected:
		PuperIo io_ = PuperIo::OUT;
	};

}