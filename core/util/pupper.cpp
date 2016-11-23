#include "pupper.h"

#include "byte_buffer.h"

namespace oak::util {

	Pupper::~Pupper() {}

	void ByteBufferPupper::pup(int8_t &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int8_t>();
		}
	}

	void ByteBufferPupper::pup(int16_t &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int16_t>();
		}
	}

	void ByteBufferPupper::pup(int32_t &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int32_t>();
		}
	}

	void ByteBufferPupper::pup(int64_t &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int64_t>();
		}
	}

	void ByteBufferPupper::pup(uint8_t &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint8_t>();
		}
	}

	void ByteBufferPupper::pup(uint16_t &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint16_t>();
		}
	}

	void ByteBufferPupper::pup(uint32_t &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint32_t>();
		}
	}

	void ByteBufferPupper::pup(uint64_t &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint64_t>();
		}
	}

	void ByteBufferPupper::pup(float &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<float>();
		}
	}

	void ByteBufferPupper::pup(double &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<double>();
		}
	}

	void ByteBufferPupper::pup(bool &data, const ObjInfo &info) {
		if (io_ == PupperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<bool>();
		}
	}

	void pup(Pupper &pupper, int8_t &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, int16_t &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, int32_t &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, int64_t &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, uint8_t &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, uint16_t &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, uint32_t &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, uint64_t &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, float &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, double &data, const ObjInfo &info) { pupper.pup(data, info); }
	void pup(Pupper &pupper, bool &data, const ObjInfo &info) { pupper.pup(data, info); }

}