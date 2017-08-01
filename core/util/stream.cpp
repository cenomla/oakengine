#include "stream.h"

namespace oak {

	template<> void Stream::write(const char *data) {
		buffer->write(strlen(data) + 1, data);
	}

	template<> void Stream::write(oak::string data) {
		buffer->write(data.size() + 1, data.c_str());
	}

	template<> oak::string Stream::read() {
		size_t n;
		char c;
		oak::string str;
		while ((n = buffer->read(1, &c)) > 0) {
			if (c == 0) { break; }
			str.push_back(c);
		}
		return str;
	}

}