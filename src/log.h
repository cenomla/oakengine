#pragma once

#include <sstream>
#include <streambuf>
#include <iostream>
#include <vector>

namespace oak {

	namespace debug {

		class Logger : public std::streambuf {
		public:
			inline void addStream(std::ostream& stream) {
				ostreams_.push_back(stream.rdbuf());
			}

		protected:
			int sync();
			std::streamsize xsputn(const char_type* s, std::streamsize count);
			int_type overflow( int_type ch = 0 );
		private:
			std::vector<std::streambuf*> ostreams_;
		};



	}

	extern debug::Logger log_stream;
	extern std::ostream log;

}