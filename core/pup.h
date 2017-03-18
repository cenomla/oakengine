#pragma once

#include "container.h"
#include "util/puper.h"

namespace oak {

	void pup(Puper &puper, int8_t &data, const ObjInfo &info);
	void pup(Puper &puper, int16_t &data, const ObjInfo &info);
	void pup(Puper &puper, int32_t &data, const ObjInfo &info);
	void pup(Puper &puper, int64_t &data, const ObjInfo &info);
	void pup(Puper &puper, uint8_t &data, const ObjInfo &info);
	void pup(Puper &puper, uint16_t &data, const ObjInfo &info);
	void pup(Puper &puper, uint32_t &data, const ObjInfo &info);
	void pup(Puper &puper, uint64_t &data, const ObjInfo &info);
	void pup(Puper &puper, float &data, const ObjInfo &info);
	void pup(Puper &puper, double &data, const ObjInfo &info);
	void pup(Puper &puper, bool &data, const ObjInfo &info);
	void pup(Puper &puper, oak::string &data, const ObjInfo &info);
	void pup(Puper &puper, glm::vec2 &data, const ObjInfo &info);
	void pup(Puper &puper, glm::vec3 &data, const ObjInfo &info);
	void pup(Puper &puper, glm::vec4 &data, const ObjInfo &info);
	void pup(Puper &puper, Entity &data, const ObjInfo &info);
	
	template<class T>
	void pup(Puper &puper, oak::vector<T> &data, const ObjInfo &info) {
		if (puper.getIo() == PuperIo::OUT) {
			size_t size = data.size();
			pup(puper, size, info + ObjInfo{ "", ObjInfo::SIZE_VAR });
			for (size_t i = 0; i < size; i++) {
				pup(puper, data[i], info + ObjInfo{ std::to_string(i + 1).c_str() });
			}
		} else {
			size_t size = 0;
			pup(puper, size, info + ObjInfo{ "", ObjInfo::SIZE_VAR });
			data.resize(size);
			for (size_t i = 0; i < size; i++) {
				pup(puper, data[i], info + ObjInfo{ std::to_string(i + 1).c_str() });
			}
		}
	}
}