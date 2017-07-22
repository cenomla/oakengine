#include "byte_buffer.h"

#include <cstring>

namespace oak {

	ByteBuffer::ByteBuffer(size_t size, Allocator *allocator) : allocator_{ allocator }, capacity_{ size }, buffer_{ nullptr }, pos_{ 0 }, mark_{ 0 }, owns_{ true } {
		init();
	}

	ByteBuffer::ByteBuffer(void *data, size_t size) : capacity_{ size }, buffer_{ static_cast<char*>(data) }, pos_{ 0 }, mark_{ 0 }, owns_{ false } {

	}

	ByteBuffer::~ByteBuffer() {
		destroy();
	}

	ByteBuffer::ByteBuffer(const ByteBuffer& other) {
		*this = other;
	}

	void ByteBuffer::operator=(const ByteBuffer& other) {
		destroy();
		capacity_ = other.capacity_;
		buffer_ = static_cast<char*>(allocator_->allocate(capacity_));
		pos_ = other.pos_;
		mark_ = other.mark_;
		owns_ = other.owns_;

		memcpy(buffer_, other.buffer_, capacity_);
	}

	ByteBuffer::ByteBuffer(ByteBuffer&& other) {
		*this = std::move(other);
	}

	void ByteBuffer::operator=(ByteBuffer&& other) {
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

	void ByteBuffer::resize(size_t nsize) {
		if (owns_ && buffer_ != nullptr) {
			if (nsize > capacity_) {
				char *temp = static_cast<char*>(allocator_->allocate(nsize));
				memcpy(temp, buffer_, capacity_);
				allocator_->deallocate(buffer_, capacity_);
				buffer_ = temp;
				capacity_ = nsize;
			}
		}
	}

	void ByteBuffer::checkResize(size_t size) {
		if (pos_ + size > capacity_) {
			resize(capacity_ * 2);
		}
	}

	void ByteBuffer::init() {
		if (buffer_ == nullptr) {
			buffer_ = static_cast<char*>(allocator_->allocate(capacity_));
		}
	}

	void ByteBuffer::destroy() {
		if (buffer_ != nullptr && owns_) {
			allocator_->deallocate(buffer_, capacity_);
			buffer_ = nullptr;
			capacity_ = 0;
			pos_ = 0;
			mark_ = 0;
		}
	}

	void ByteBuffer::read(size_t size, void *data) {
		memcpy(data, buffer_ + pos_, size);
		pos_ += size;
	}
	
	void ByteBuffer::write(size_t size, const void *data) {
		checkResize(size);
		memcpy(buffer_ + pos_, data, size);
		pos_ += size;
	}

}