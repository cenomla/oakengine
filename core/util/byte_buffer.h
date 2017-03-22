#pragma once

#include <cstddef>
#include <cinttypes>
#include <cstring>

#include "container.h"

namespace oak::util {

	class ByteBuffer {
	public:
		ByteBuffer(size_t size);
		ByteBuffer(void *data, size_t size);
		~ByteBuffer();
		
		ByteBuffer(const ByteBuffer& other);
		void operator=(const ByteBuffer& oter);

		ByteBuffer(ByteBuffer&& other);
		void operator=(ByteBuffer&& other);

		template<class T> void write(T data);
		template<class T> T read();

		void set();
		void reset();
		void rewind();

		inline size_t capacity() const { return capacity_; }
		inline size_t pos() const { return pos_; }

		inline char* data() { return buffer_; }
		inline const char* data() const { return buffer_; }
	private:
		size_t capacity_;
		char *buffer_;
		size_t pos_;
		size_t mark_;
		bool owns_;

		void init();
		void destroy();
	};

	template<> void ByteBuffer::write(int8_t data);
	template<> void ByteBuffer::write(int16_t data);
	template<> void ByteBuffer::write(int32_t data);
	template<> void ByteBuffer::write(int64_t data);

	template<> void ByteBuffer::write(uint8_t data);
	template<> void ByteBuffer::write(uint16_t data);
	template<> void ByteBuffer::write(uint32_t data);
	template<> void ByteBuffer::write(uint64_t data);

	template<> void ByteBuffer::write(float data);
	template<> void ByteBuffer::write(const char* data);

	template<> int8_t ByteBuffer::read();
	template<> int16_t ByteBuffer::read();
	template<> int32_t ByteBuffer::read();
	template<> int64_t ByteBuffer::read();

	template<> uint8_t ByteBuffer::read();
	template<> uint16_t ByteBuffer::read();
	template<> uint32_t ByteBuffer::read();
	template<> uint64_t ByteBuffer::read();

	template<> float ByteBuffer::read();
	template<> oak::string ByteBuffer::read();

}