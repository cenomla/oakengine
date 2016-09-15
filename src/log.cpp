#include "log.h"

namespace oak::log {

	Logger cout_stream{ 64 };
	std::ostream cout(&cout_stream);

	Logger::Logger(size_t buffSize) : buff_(buffSize+1) {
		setp(buff_.data(), buff_.data() + buffSize);
	}

	bool Logger::writeAndFlush() {
		std::ptrdiff_t n = pptr() - pbase();

		pbump(-n);

		bool success = true;
		bool s = true;

		for (auto it : ostreams_) {
			it->write(pbase(), n);
			if (success != false) { success = s; }
		}

		return success;
	}

	int Logger::sync() {
		return writeAndFlush() ? 0 : -1;
	}

	std::streambuf::int_type Logger::overflow( std::streambuf::int_type ch ) {
		//std::cout << "overflow: " << ch << std::endl;
		if (ostreams_.size() > 0 && ch != traits_type::eof()) {
			*pptr() = ch;
			pbump(1);
			if (writeAndFlush()) {
				return ch;
			}
		}

		return traits_type::eof();
	}

}