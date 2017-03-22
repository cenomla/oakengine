#include "log.h"

#include <cstring>

namespace oak::log {

	Logger cout{ "stdout" };
	Logger cwarn{ "stdwarn" };
	Logger cerr{ "stderr" };

	Logger::Logger(const char* name) : name_{ name } {

	}

	Logger::~Logger() {
		for (auto& stream : streams_) {
			stream->close();
		}
	}

	void Logger::addStream(Stream *stream) {
		stream->open();
		streams_.push_back(stream);
	}

	void Logger::print(const char* text, Level level, const char* file, int line) {
		switch (level) {
		case Level::MINIMAL:
			sprintf(buffer_, "[%s]: %s\n", name_, text);
		break;
		case Level::NORMAL:
			sprintf(buffer_, "[%s]: %s\n", name_, text);
		break;
		case Level::VERBOSE:
			sprintf(buffer_, "file: %s, line: %i [%s]: %s\n", file, line, name_, text);
		break;
		}
		flush();
	}

	void Logger::flush() {
		for (auto& stream : streams_) {
			stream->write(buffer_, strlen(buffer_));
		}
	}

}