#pragma once

#include <cstring>
#include <climits>

#include "container.h"

namespace oak {

	template <class T>
	T swap_endian(T u) {
		if constexpr(1) {
			static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

			union {
				T u;
				unsigned char u8[sizeof(T)];
			} source, dest;

			source.u = u;

			for (size_t i = 0; i < sizeof(T); i++) {
				dest.u8[i] = source.u8[sizeof(T) - i - 1];
			}

			return dest.u;
		} else {
			return u;
		}
	}

	class BufferBase {
	public:
		virtual ~BufferBase() {};

		virtual void set() = 0;
		virtual void reset() = 0;
		virtual void rewind() = 0;
		
		virtual size_t read(size_t size, void *data) = 0;
		virtual size_t write(size_t size, const void *data) = 0;

		virtual size_t size() const = 0;
	};

	struct Stream {
		BufferBase *buffer;

		template<class T> T read() {
			T d;
			buffer->read(sizeof(T), &d);
			return swap_endian<T>(d);
		}
		
		template<class T> void write(T value) {
			const T& e = swap_endian<T>(value);
			buffer->write(sizeof(T), &e);
		}
	};

	template<> void Stream::write(oak::string data);
	template<> void Stream::write(const char* data);

	template<> oak::string Stream::read();

}