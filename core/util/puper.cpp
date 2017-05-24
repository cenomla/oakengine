#include "puper.h"

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
	void pup(Puper& puper, oak::string& data, const ObjInfo& info) { puper.pup(data, info); }
	
	void pup(Puper& puper, glm::vec2& data, const ObjInfo& info) {
		puper.pup(data.x, info + ObjInfo{ "x" });
		puper.pup(data.y, info + ObjInfo{ "y" });
	}
	void pup(Puper& puper, glm::vec3& data, const ObjInfo& info) {
		puper.pup(data.x, info + ObjInfo{ "x" });
		puper.pup(data.y, info + ObjInfo{ "y" });
		puper.pup(data.z, info + ObjInfo{ "z" });
	}
	void pup(Puper& puper, glm::vec4& data, const ObjInfo& info) {
		puper.pup(data.x, info + ObjInfo{ "x" });
		puper.pup(data.y, info + ObjInfo{ "y" });
		puper.pup(data.z, info + ObjInfo{ "z" });
		puper.pup(data.w, info + ObjInfo{ "w" });
	}

	void pup(Puper& puper, glm::mat4& data, const ObjInfo& info) {
	}

}