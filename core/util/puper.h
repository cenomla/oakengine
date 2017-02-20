#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>

namespace oak {

	struct ObjInfo {
		constexpr static uint32_t HAS_NAME = 0x00000001;
		constexpr static uint32_t HAS_INDEX = 0x00000002;
		constexpr static uint32_t SIZE_VAR = 0x00000004;

		std::string name;
		int index = -1;
		uint32_t flags = 0;

		inline const ObjInfo& combine(const ObjInfo &other) {
			if (!other.name.empty()) {
				name = other.name + "." + name;
			}
			return *this;
		}
	};

	inline const ObjInfo& operator+(ObjInfo &&lhs, const ObjInfo &rhs) {
		return lhs.combine(rhs);
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
		virtual void pup(std::string &data, const ObjInfo &info) = 0;
		virtual void pup(Entity &data, const ObjInfo &info) = 0;
	
		inline void setIo(PuperIo io) { io_ = io; }
		inline PuperIo getIo() const { return io_; }
	protected:
		PuperIo io_ = PuperIo::OUT;
	};

}