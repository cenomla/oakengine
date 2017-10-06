#include "byte_buffer.h"

#include <cstring>

namespace oak {

	void ByteBuffer::init(size_t size) {
		storage_.init(capacity_);
	}

	void ByteBuffer::destroy() {
		storage_.destroy();
		pos_ = 0;
		mark_ = 0;
	}

	void ByteBuffer::set() {
		mark_ = pos_;
	}

	void ByteBuffer::reset() {
		pos_ = mark_;
		mark_ = 0;
	}

	void ByteBuffer::rewind() {
		pos_ = 0;
	}

	void ByteBuffer::resize(size_t nsize) {
		storage_.resize(nsize);
	}

	void ByteBuffer::checkResize(size_t size) {
		if (pos_ + size > storage_.capacity) {
			resize(storage_.capacity * 2);
		}
	}

	size_t ByteBuffer::read(size_t size, void *data) {
		if (pos_ + size > storage_.capacity) {
			size = storage_.capacity - pos_;
		}
		if (size == 0) { return 0; }
		memcpy(data, storage_.data + pos_, size);
		pos_ += size;
		return size;
	}
	
	size_t ByteBuffer::write(size_t size, const void *data) {
		if (size == 0) { return 0; }
		checkResize(size);
		memcpy(storage_.data + pos_, data, size);
		pos_ += size;
		return size;
	}

}
