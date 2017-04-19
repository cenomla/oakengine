#include "buffer_puper.h"

#include "util/byte_buffer.h"

namespace oak {

	BufferPuper::BufferPuper(util::ByteBuffer *buffer) : buffer_{ buffer } {}

	void BufferPuper::pup(int8_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int8_t>();
		}
	}

	void BufferPuper::pup(int16_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int16_t>();
		}
	}

	void BufferPuper::pup(int32_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int32_t>();
		}
	}

	void BufferPuper::pup(int64_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int64_t>();
		}
	}

	void BufferPuper::pup(uint8_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint8_t>();
		}
	}

	void BufferPuper::pup(uint16_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint16_t>();
		}
	}

	void BufferPuper::pup(uint32_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint32_t>();
		}
	}

	void BufferPuper::pup(uint64_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint64_t>();
		}
	}

	void BufferPuper::pup(float& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<float>();
		}
	}

	void BufferPuper::pup(double& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<double>();
		}
	}

	void BufferPuper::pup(bool& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(static_cast<uint8_t>(data));
		} else {
			data = static_cast<bool>(buffer_->read<uint8_t>());
		}
	}

	void BufferPuper::pup(oak::string& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<oak::string>();
		}
	}

}