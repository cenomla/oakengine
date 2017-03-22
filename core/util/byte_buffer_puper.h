#pragma once

#include <cstdint>

#include "util/puper.h"

namespace oak {

	namespace util { class ByteBuffer; }

	class ByteBufferPuper : public Puper {
	public:
		ByteBufferPuper(util::ByteBuffer *buffer);

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
		void pup(Entity& data, const ObjInfo& info) override;

	private:
		util::ByteBuffer *buffer_;
	};

}