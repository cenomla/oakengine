#include "puper.h"

#include "byte_buffer.h"

namespace oak {

	Puper::~Puper() {}

	void pup(Puper &puper, int8_t &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, int16_t &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, int32_t &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, int64_t &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, uint8_t &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, uint16_t &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, uint32_t &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, uint64_t &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, float &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, double &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, bool &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, oak::string &data, const ObjInfo &info) { puper.pup(data, info); }
	void pup(Puper &puper, Entity &data, const ObjInfo &info) { puper.pup(data, info); }
	
	void pup(Puper &puper, glm::vec2 &data, const ObjInfo &info) {
		puper.pup(data.x, ObjInfo{ "x" } + info);
		puper.pup(data.y, ObjInfo{ "y" } + info);
	}
	void pup(Puper &puper, glm::vec3 &data, const ObjInfo &info) {
		puper.pup(data.x, ObjInfo{ "x" } + info);
		puper.pup(data.y, ObjInfo{ "y" } + info);
		puper.pup(data.z, ObjInfo{ "z" } + info);
	}
	void pup(Puper &puper, glm::vec4 &data, const ObjInfo &info) {
		puper.pup(data.x, ObjInfo{ "x" } + info);
		puper.pup(data.y, ObjInfo{ "y" } + info);
		puper.pup(data.z, ObjInfo{ "z" } + info);
		puper.pup(data.w, ObjInfo{ "w" } + info);
	}

}