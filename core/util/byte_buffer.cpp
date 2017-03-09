#include "byte_buffer.h"

#include <cstring>

#include "memory/memory_manager.h"

namespace oak::util {

	ByteBuffer::ByteBuffer(size_t size) : capacity_{ size }, buffer_{ nullptr }, pos_{ 0 }, mark_{ 0 }, owns_{ true } {
		init();
	}

	ByteBuffer::ByteBuffer(void *data, size_t size) : capacity_{ size }, buffer_{ static_cast<char*>(data) }, pos_{ 0 }, mark_{ 0 }, owns_{ false } {

	}

	ByteBuffer::~ByteBuffer() {
		destroy();
	}

	ByteBuffer::ByteBuffer(const ByteBuffer &other) {
		*this = other;
	}

	void ByteBuffer::operator=(const ByteBuffer &other) {
		destroy();
		capacity_ = other.capacity_;
		buffer_ = static_cast<char*>(MemoryManager::inst().allocate(capacity_));
		pos_ = other.pos_;
		mark_ = other.mark_;
		owns_ = other.owns_;

		memcpy(buffer_, other.buffer_, capacity_);

	}

	ByteBuffer::ByteBuffer(ByteBuffer &&other) {
		*this = std::move(other);
	}

	void ByteBuffer::operator=(ByteBuffer &&other) {
		capacity_ = other.capacity_;
		buffer_ = other.buffer_;
		pos_ = other.pos_;
		mark_ = other.mark_;
		owns_ = other.owns_;

		other.capacity_ = 0;
		other.buffer_ = nullptr;
		other.pos_ = 0;
		other.mark_ = 0;
		other.owns_ = false;
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

	void ByteBuffer::init() {
		if (buffer_ == nullptr) {
			buffer_ = static_cast<char*>(MemoryManager::inst().allocate(capacity_));
		}
	}

	void ByteBuffer::destroy() {
		if (buffer_ != nullptr && owns_) {
			MemoryManager::inst().deallocate(buffer_, capacity_);
			buffer_ = nullptr;
		}
	}

}