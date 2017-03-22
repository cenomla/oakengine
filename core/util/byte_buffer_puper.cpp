#include "byte_buffer_puper.h"

#include "util/byte_buffer.h"
#include "entity.h"

namespace oak {

	ByteBufferPuper::ByteBufferPuper(util::ByteBuffer *buffer) : buffer_{ buffer } {}

	void ByteBufferPuper::pup(int8_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int8_t>();
		}
	}

	void ByteBufferPuper::pup(int16_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int16_t>();
		}
	}

	void ByteBufferPuper::pup(int32_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int32_t>();
		}
	}

	void ByteBufferPuper::pup(int64_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<int64_t>();
		}
	}

	void ByteBufferPuper::pup(uint8_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint8_t>();
		}
	}

	void ByteBufferPuper::pup(uint16_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint16_t>();
		}
	}

	void ByteBufferPuper::pup(uint32_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint32_t>();
		}
	}

	void ByteBufferPuper::pup(uint64_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<uint64_t>();
		}
	}

	void ByteBufferPuper::pup(float& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<float>();
		}
	}

	void ByteBufferPuper::pup(double& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<double>();
		}
	}

	void ByteBufferPuper::pup(bool& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(static_cast<uint8_t>(data));
		} else {
			data = static_cast<bool>(buffer_->read<uint8_t>());
		}
	}

	void ByteBufferPuper::pup(oak::string& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data);
		} else {
			data = buffer_->read<oak::string>();
		}
	}

	void ByteBufferPuper::pup(Entity& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			buffer_->write(data.id());
			buffer_->write(reinterpret_cast<uintptr_t>(data.getManager()));
		} else {
			data = { buffer_->read<uint64_t>(), reinterpret_cast<EntityManager*>(buffer_->read<uintptr_t>()) };
		}
	}

}