#pragma once

#include <glm/glm.hpp>
#include <type_traits>

#include "container.h"

namespace oak {

	struct ObjInfo {
		static constexpr uint32_t VOLATILE = 0x01; //data will not be the same across instances (eg. a pointer)
		static constexpr uint32_t LENGTH = 0x02; //data will describe the parents length
		
		template<class T>
		inline static ObjInfo make(const ObjInfo *p, const oak::string& n, uint32_t f = 0) {
			return ObjInfo{ p, n, sizeof(T), f | (std::is_pointer_v<T> ? VOLATILE : 0) };
		}

		const ObjInfo *parent;
		oak::string name;
		size_t size = 0;
		uint32_t flags = 0;
	};

	enum class PuperIo {
		IN,
		OUT
	};

	class Puper {
	public:
		virtual ~Puper();

		virtual void pup(int8_t& data, const ObjInfo& info) = 0;
		virtual void pup(int16_t& data, const ObjInfo& info) = 0;
		virtual void pup(int32_t& data, const ObjInfo& info) = 0;
		virtual void pup(int64_t& data, const ObjInfo& info) = 0;
		virtual void pup(uint8_t& data, const ObjInfo& info) = 0;
		virtual void pup(uint16_t& data, const ObjInfo& info) = 0;
		virtual void pup(uint32_t& data, const ObjInfo& info) = 0;
		virtual void pup(uint64_t& data, const ObjInfo& info) = 0;
		virtual void pup(float& data, const ObjInfo& info) = 0;
		virtual void pup(double& data, const ObjInfo& info) = 0;
		virtual void pup(bool& data, const ObjInfo& info) = 0;
		virtual void pup(void*& data, const ObjInfo& info) = 0;
		virtual void pup(oak::string& data, const ObjInfo& info) = 0;
	
		void setIo(PuperIo io) { io_ = io; }
	protected:
		PuperIo io_ = PuperIo::OUT;

	};

}