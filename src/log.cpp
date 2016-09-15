#include "log.h"

namespace oak {

	debug::Logger log_stream;
	std::ostream log(&log_stream);

	namespace debug {

		int Logger::sync() {
			for (auto it : ostreams_) {
				it->pubsync();
			}
			return 0;
		}

		std::streamsize Logger::xsputn(const char_type *s, std::streamsize count) {
			for (auto it : ostreams_) {
				it->sputn(s, count);
			}
			return count;
		}

		std::streambuf::int_type Logger::overflow( std::streambuf::int_type ch ) {
			for (auto it : ostreams_) {
			//	it->overflow(ch);
			}

			return ch;
		}


	}

}