#include "stream_puper.h"

#include "util/stream.h"

namespace oak {

	StreamPuper::StreamPuper(Stream *stream) : stream_{ stream } {}

	void StreamPuper::pup(int8_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<int8_t>();
		}
	}

	void StreamPuper::pup(int16_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<int16_t>();
		}
	}

	void StreamPuper::pup(int32_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<int32_t>();
		}
	}

	void StreamPuper::pup(int64_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<int64_t>();
		}
	}

	void StreamPuper::pup(uint8_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<uint8_t>();
		}
	}

	void StreamPuper::pup(uint16_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<uint16_t>();
		}
	}

	void StreamPuper::pup(uint32_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<uint32_t>();
		}
	}

	void StreamPuper::pup(uint64_t& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<uint64_t>();
		}
	}

	void StreamPuper::pup(float& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<float>();
		}
	}

	void StreamPuper::pup(double& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<double>();
		}
	}

	void StreamPuper::pup(bool& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(static_cast<uint8_t>(data));
		} else {
			data = static_cast<bool>(stream_->read<uint8_t>());
		}
	}

	void StreamPuper::pup(oak::string& data, const ObjInfo& info) {
		if (io_ == PuperIo::OUT) {
			stream_->write(data);
		} else {
			data = stream_->read<oak::string>();
		}
	}

}