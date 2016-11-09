#include "log.h"

namespace oak::log {

	Logger cout_stream{ 64 };
	std::ostream cout(&cout_stream);

	Logger::Logger(size_t buffSize) : buff_(buffSize) {
		setp(buff_.data(), buff_.data() + buffSize-1);
	}

	bool Logger::writeAndFlush() {
		std::ptrdiff_t n = pptr() - pbase();

		pbump(-n);

		bool success = true;
		std::ios_base::iostate s;

		for (auto it : ostreams_) {
			s = it->write(pbase(), n).rdstate();
			if (s != std::ios_base::goodbit) {
				success = false;
			}
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