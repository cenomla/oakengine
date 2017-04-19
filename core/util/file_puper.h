#pragma once

#include <cstdio>

#include "puper.h"

namespace oak {

	class FilePuper : public Puper {
	public:
		FilePuper(FILE *file);

		void pup(int8_t& data, const ObjInfo& info) override;
		void pup(int16_t& data, const ObjInfo& info) override;
		void pup(int32_t& data, const ObjInfo& info) override;
		void pup(int64_t& data, const ObjInfo& info) override;
		void pup(uint8_t& data, const ObjInfo& info) override;
		void pup(uint16_t& data, const ObjInfo& info) override;
		void pup(uint32_t& data, const ObjInfo& info) override;
		void pup(uint64_t& data, const ObjInfo& info) override;
		void pup(float& data, const ObjInfo& info) override;
		void pup(double& data, const ObjInfo& info) override;
		void pup(bool& data, const ObjInfo& info) override;
		void pup(oak::string& data, const ObjInfo& info) override;

	private:
		FILE *file_;
	};

}