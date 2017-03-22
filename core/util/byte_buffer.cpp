#include "byte_buffer.h"

#include <cstring>

#include "memory/alloc.h"

namespace oak::util {

	ByteBuffer::ByteBuffer(size_t size) : capacity_{ size }, buffer_{ nullptr }, pos_{ 0 }, mark_{ 0 }, owns_{ true } {
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
		buffer_ = static_cast<char*>(proxyAllocator.allocate(capacity_));
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

	void ByteBuffer::init() {
		if (buffer_ == nullptr) {
			buffer_ = static_cast<char*>(proxyAllocator.allocate(capacity_));
		}
	}

	void ByteBuffer::destroy() {
		if (buffer_ != nullptr && owns_) {
			proxyAllocator.deallocate(buffer_, capacity_);
			buffer_ = nullptr;
		}
	}

	template<> void ByteBuffer::write(int8_t data) {
		(buffer_ + pos_)[0] = static_cast<char>(data);
		pos_ += 1;
	}
	template<> void ByteBuffer::write(int16_t data) {
		(buffer_ + pos_)[0] = static_cast<char>(data & 0xFF);
		(buffer_ + pos_)[1] = static_cast<char>((data >> 8) & 0xFF);
		pos_ += 2;
	}
	template<> void ByteBuffer::write(int32_t data) {
		(buffer_ + pos_)[0] = static_cast<char>(data & 0xFF);
		(buffer_ + pos_)[1] = static_cast<char>((data >> 8) & 0xFF);
		(buffer_ + pos_)[2] = static_cast<char>((data >> 16) & 0xFF);
		(buffer_ + pos_)[3] = static_cast<char>((data >> 24) & 0xFF);
		pos_ += 4;
	}
	template<> void ByteBuffer::write(int64_t data) {
		(buffer_ + pos_)[0] = static_cast<char>(data & 0xFF);
		(buffer_ + pos_)[1] = static_cast<char>((data >> 8) & 0xFF);
		(buffer_ + pos_)[2] = static_cast<char>((data >> 16) & 0xFF);
		(buffer_ + pos_)[3] = static_cast<char>((data >> 24) & 0xFF);
		(buffer_ + pos_)[4] = static_cast<char>((data >> 32) & 0xFF);
		(buffer_ + pos_)[5] = static_cast<char>((data >> 40) & 0xFF);
		(buffer_ + pos_)[6] = static_cast<char>((data >> 48) & 0xFF);
		(buffer_ + pos_)[7] = static_cast<char>((data >> 56) & 0xFF);
		pos_ += 8;
	}

	template<> void ByteBuffer::write(uint8_t data) {
		(buffer_ + pos_)[0] = static_cast<char>(data);
		pos_ += 1;
	}
	template<> void ByteBuffer::write(uint16_t data) {
		(buffer_ + pos_)[0] = static_cast<char>(data & 0xFF);
		(buffer_ + pos_)[1] = static_cast<char>((data >> 8) & 0xFF);
		pos_ += 2;
	}
	template<> void ByteBuffer::write(uint32_t data) {
		(buffer_ + pos_)[0] = static_cast<char>(data & 0xFF);
		(buffer_ + pos_)[1] = static_cast<char>((data >> 8) & 0xFF);
		(buffer_ + pos_)[2] = static_cast<char>((data >> 16) & 0xFF);
		(buffer_ + pos_)[3] = static_cast<char>((data >> 24) & 0xFF);
		pos_ += 4;
	}
	template<> void ByteBuffer::write(uint64_t data) {
		(buffer_ + pos_)[0] = static_cast<char>(data & 0xFF);
		(buffer_ + pos_)[1] = static_cast<char>((data >> 8) & 0xFF);
		(buffer_ + pos_)[2] = static_cast<char>((data >> 16) & 0xFF);
		(buffer_ + pos_)[3] = static_cast<char>((data >> 24) & 0xFF);
		(buffer_ + pos_)[4] = static_cast<char>((data >> 32) & 0xFF);
		(buffer_ + pos_)[5] = static_cast<char>((data >> 40) & 0xFF);
		(buffer_ + pos_)[6] = static_cast<char>((data >> 48) & 0xFF);
		(buffer_ + pos_)[7] = static_cast<char>((data >> 56) & 0xFF);
		pos_ += 8;
	}

	template<> void ByteBuffer::write(float data) {
		float *buff = reinterpret_cast<float*>(buffer_ + pos_);
		*buff = data;
		pos_ += 4;
	}

	template<> void ByteBuffer::write(const char * data) {
		const char *str = data;
		strcpy(buffer_ + pos_, str);
		pos_ += strlen(str) + 1;
	}

	template<> int8_t ByteBuffer::read() {
		int8_t data = static_cast<int8_t>((buffer_ + pos_)[0]);
		pos_ += 1;
		return data;
	}
	template<> int16_t ByteBuffer::read() {
		int16_t data =
			((static_cast<int16_t>((buffer_ + pos_)[1]) & 0xFF) << 8) | 
			(static_cast<int16_t>((buffer_ + pos_)[0]) & 0xFF);
		pos_ += 2;
		return data;
	}
	template<> int32_t ByteBuffer::read() {
		int32_t data = 
			((static_cast<int32_t>((buffer_ + pos_)[3]) & 0xFF) << 24) | 
			((static_cast<int32_t>((buffer_ + pos_)[2]) & 0xFF) << 16) | 
			((static_cast<int32_t>((buffer_ + pos_)[1]) & 0xFF) << 8) | 
			(static_cast<int32_t>((buffer_ + pos_)[0]) & 0xFF);
		pos_ += 4;
		return data;
	}
	template<> int64_t ByteBuffer::read()  {
		int64_t data = 
			((static_cast<int64_t>((buffer_ + pos_)[7]) & 0xFF) << 56) | 
			((static_cast<int64_t>((buffer_ + pos_)[6]) & 0xFF) << 48) | 
			((static_cast<int64_t>((buffer_ + pos_)[5]) & 0xFF) << 40) | 
			((static_cast<int64_t>((buffer_ + pos_)[4]) & 0xFF) << 32) | 
			((static_cast<int64_t>((buffer_ + pos_)[3]) & 0xFF) << 24) | 
			((static_cast<int64_t>((buffer_ + pos_)[2]) & 0xFF) << 16) | 
			((static_cast<int64_t>((buffer_ + pos_)[1]) & 0xFF) << 8) | 
			(static_cast<int64_t>((buffer_ + pos_)[0]) & 0xFF);
		pos_ += 8;
		return data;
	}

	template<> uint8_t ByteBuffer::read() {
		uint8_t data = static_cast<uint8_t>((buffer_ + pos_)[0]);
		pos_ += 1;
		return data;
	}
	template<> uint16_t ByteBuffer::read() {
		uint16_t data =
			((static_cast<uint16_t>((buffer_ + pos_)[1]) & 0xFF) << 8) | 
			(static_cast<uint16_t>((buffer_ + pos_)[0]) & 0xFF);
		pos_ += 2;
		return data;
	}
	template<> uint32_t ByteBuffer::read() {
		uint32_t data = 
			((static_cast<uint32_t>((buffer_ + pos_)[3]) & 0xFF) << 24) | 
			((static_cast<uint32_t>((buffer_ + pos_)[2]) & 0xFF) << 16) | 
			((static_cast<uint32_t>((buffer_ + pos_)[1]) & 0xFF) << 8) | 
			(static_cast<uint32_t>((buffer_ + pos_)[0]) & 0xFF);
		pos_ += 4;
		return data;
	}
	template<> uint64_t ByteBuffer::read() {
		uint64_t data = 
			((static_cast<uint64_t>((buffer_ + pos_)[7]) & 0xFF) << 56) | 
			((static_cast<uint64_t>((buffer_ + pos_)[6]) & 0xFF) << 48) | 
			((static_cast<uint64_t>((buffer_ + pos_)[5]) & 0xFF) << 40) | 
			((static_cast<uint64_t>((buffer_ + pos_)[4]) & 0xFF) << 32) | 
			((static_cast<uint64_t>((buffer_ + pos_)[3]) & 0xFF) << 24) | 
			((static_cast<uint64_t>((buffer_ + pos_)[2]) & 0xFF) << 16) | 
			((static_cast<uint64_t>((buffer_ + pos_)[1]) & 0xFF) << 8) | 
			(static_cast<uint64_t>((buffer_ + pos_)[0]) & 0xFF);
		pos_ += 8;
		return data;
	}

	template<> float ByteBuffer::read() {
		float data = *reinterpret_cast<float*>(buffer_ + pos_);
		pos_ += 4;
		return data;
	}

	template<> oak::string ByteBuffer::read() {
		const char *str = buffer_ + pos_;
		pos_ += strlen(str) + 1;
		return { str };
	}

}