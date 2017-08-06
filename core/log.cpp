#include "log.h"

#include <ctime>
#include <cstring>
#include <algorithm>

namespace oak::log {

	Logger cout{ "msg" };
	Logger cwarn{ "warn" };
	Logger cerr{ "err" };

	Logger::Logger(const char* name) : name_{ name } {

	}

	void Logger::addStream(Stream *stream) {
		streams_.push_back(stream);
	}

	void Logger::removeStream(Stream *stream) {
		streams_.erase(std::remove(std::begin(streams_), std::end(streams_), stream), std::end(streams_));
	}

	void Logger::print(const char* text, Level level, const char* file, int line) {
		memset(buffer_, 0, 2048);
		//get time info
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		switch (level) {
		case Level::MINIMAL:
			sprintf(buffer_, "[%s]: %s\n", name_, text);
		break;
		case Level::NORMAL:
			sprintf(buffer_, "[%i:%i:%i %i/%i/%i][%s]: %s\n", 
				tm->tm_hour, tm->tm_min, tm->tm_sec, 
				tm->tm_mon+1, tm->tm_mday, 1900+tm->tm_year, 
				name_, text);
		break;
		case Level::VERBOSE:
			sprintf(buffer_, "[%i:%i:%i %i/%i/%i][file]:%s:%i[%s]: %s\n", 
				tm->tm_hour, tm->tm_min, tm->tm_sec,
				tm->tm_mon+1, tm->tm_mday, 1900+tm->tm_year, 
				file, line, name_, text);
		break;
		}
		flush();
	}

	void Logger::flush() {

		for (auto& stream : streams_) {
			stream->buffer->write(strlen(buffer_), buffer_);
		}
	}

}