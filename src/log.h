#pragma once

#include <sstream>
#include <streambuf>
#include <iostream>
#include <vector>

namespace oak::log {

	class Logger : public std::streambuf {
	public:
		Logger(size_t buffSize);

		inline void addStream(std::ostream& stream) {
			stream.clear();
			ostreams_.push_back(&stream);
		}

	protected:
		bool writeAndFlush();

		int sync();
		int_type overflow( int_type ch = traits_type::eof() );
	private:
		std::vector<char> buff_;
		std::vector<std::ostream*> ostreams_;
	};

	extern Logger cout_stream;
	extern std::ostream cout;

}