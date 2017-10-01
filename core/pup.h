#pragma once

#include "container.h"
#include "math.h"
#include "util/puper.h"

namespace oak {

	void pup(Puper& puper, int8_t& data, const ObjInfo& info);
	void pup(Puper& puper, int16_t& data, const ObjInfo& info);
	void pup(Puper& puper, int32_t& data, const ObjInfo& info);
	void pup(Puper& puper, int64_t& data, const ObjInfo& info);
	void pup(Puper& puper, uint8_t& data, const ObjInfo& info);
	void pup(Puper& puper, uint16_t& data, const ObjInfo& info);
	void pup(Puper& puper, uint32_t& data, const ObjInfo& info);
	void pup(Puper& puper, uint64_t& data, const ObjInfo& info);
	void pup(Puper& puper, float& data, const ObjInfo& info);
	void pup(Puper& puper, double& data, const ObjInfo& info);
	void pup(Puper& puper, bool& data, const ObjInfo& info);
	void pup(Puper& puper, void*& data, const ObjInfo& info);
	void pup(Puper& puper, oak::string& data, const ObjInfo& info);
	void pup(Puper& puper, Vec2& data, const ObjInfo& info);
	void pup(Puper& puper, Vec3& data, const ObjInfo& info);
	void pup(Puper& puper, Vec4& data, const ObjInfo& info);
	void pup(Puper& puper, Mat3& data, const ObjInfo& info);
	void pup(Puper& puper, Mat4& data, const ObjInfo& info);

	template<class T>
	void pup(Puper& puper, oak::vector<T>& data, const ObjInfo& info) {
		size_t size = data.size();
		pup(puper, size, ObjInfo::make<size_t>(&info, "size", ObjInfo::LENGTH));
		data.resize(size);
		for (size_t i = 0; i < size; i++) {
			pup(puper, data[i], ObjInfo::make<T>(&info, std::to_string(i).c_str()));
		}
	}
}
