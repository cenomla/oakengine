#pragma once

#include <cstddef>

namespace oak::util {

	class ByteBuffer {
	public:
		ByteBuffer(size_t size);
		~ByteBuffer();
		
		ByteBuffer(const ByteBuffer &other);
		void operator=(const ByteBuffer &oter);

		ByteBuffer(ByteBuffer &&other);
		void operator=(ByteBuffer &&other);

		template<class T>
		void write(T&& data) {
			memcpy(buffer_ + pos_, &data, sizeof(T));
			pos_ += sizeof(T);
		}

		template<class T>
		T read() {
			T data;
			memcpy(&data, buffer_ + pos_, sizeof(T));
			pos_ += sizeof(T);
			return data;
		}

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

		void init();
		void destroy();
	};

}