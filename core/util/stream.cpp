#include "stream.h"

namespace oak {

	template<> void Stream::write(const char *data) {
		buffer->write(strlen(data), data);
	}

	template<> void Stream::write(oak::string data) {
		buffer->write(data.size(), data.c_str());
	}

	template<> oak::string Stream::read() {
		char c;
		oak::string str;
		do {
			buffer->read(1, &c);
			str.push_back(c);
		} while (c != '\0');
		return str;
	}

}