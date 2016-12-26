#include "puper.h"

#include "byte_buffer.h"

namespace oak::util {

	Puper::~Puper() {}

	ByteBufferPuper::ByteBufferPuper(ByteBuffer &buffer) : buffer_{ &buffer } {}

	void ByteBufferPuper::pup(int8_t &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int8_t>();
		}
	}

	void ByteBufferPuper::pup(int16_t &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int16_t>();
		}
	}

	void ByteBufferPuper::pup(int32_t &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int32_t>();
		}
	}

	void ByteBufferPuper::pup(int64_t &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int64_t>();
		}
	}

	void ByteBufferPuper::pup(uint8_t &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint8_t>();
		}
	}

	void ByteBufferPuper::pup(uint16_t &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint16_t>();
		}
	}

	void ByteBufferPuper::pup(uint32_t &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint32_t>();
		}
	}

	void ByteBufferPuper::pup(uint64_t &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint64_t>();
		}
	}

	void ByteBufferPuper::pup(float &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<float>();
		}
	}

	void ByteBufferPuper::pup(double &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<double>();
		}
	}

	void ByteBufferPuper::pup(bool &data, const ObjInfo &info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<bool>();
		}
	}

	void ByteBufferPuper::pup(Entity &data, const ObjInfo &info) {
		
	}

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

	void pup(Puper &puper, Entity &data, const ObjInfo &info) {
		puper.pup(data, info);
	}

}