#include "puper.h"
#include "pup.h"

namespace oak {

	Puper::~Puper() {}

	void pup(Puper& puper, int8_t& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, int16_t& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, int32_t& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, int64_t& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, uint8_t& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, uint16_t& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, uint32_t& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, uint64_t& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, float& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, double& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, bool& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, void*& data, const ObjInfo& info) { puper.pup(data, info); }
	void pup(Puper& puper, oak::string& data, const ObjInfo& info) { puper.pup(data, info); }
	
	void pup(Puper& puper, Vec2& data, const ObjInfo& info) {
		pup(puper, data.x, ObjInfo::make<float>(&info, "x"));
		pup(puper, data.y, ObjInfo::make<float>(&info, "y"));
	}
	void pup(Puper& puper, Vec3& data, const ObjInfo& info) {
		pup(puper, data.x, ObjInfo::make<float>(&info, "x"));
		pup(puper, data.y, ObjInfo::make<float>(&info, "y"));
		pup(puper, data.z, ObjInfo::make<float>(&info, "z"));
	}
	void pup(Puper& puper, Vec4& data, const ObjInfo& info) {
		pup(puper, data.x, ObjInfo::make<float>(&info, "x"));
		pup(puper, data.y, ObjInfo::make<float>(&info, "y"));
		pup(puper, data.z, ObjInfo::make<float>(&info, "z"));
		pup(puper, data.w, ObjInfo::make<float>(&info, "w"));
	}

	void pup(Puper& puper, Mat3& data, const ObjInfo& info) {
		pup(puper, data.value[0], ObjInfo::make<Vec3>(&info, "[0]"));
		pup(puper, data.value[1], ObjInfo::make<Vec3>(&info, "[1]"));
		pup(puper, data.value[2], ObjInfo::make<Vec3>(&info, "[2]"));
	}

	void pup(Puper& puper, Mat4& data, const ObjInfo& info) {
		pup(puper, data.value[0], ObjInfo::make<Vec4>(&info, "[0]"));
		pup(puper, data.value[1], ObjInfo::make<Vec4>(&info, "[1]"));
		pup(puper, data.value[2], ObjInfo::make<Vec4>(&info, "[2]"));
		pup(puper, data.value[3], ObjInfo::make<Vec4>(&info, "[3]"));
	}

}
