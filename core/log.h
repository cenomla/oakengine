#pragma once

#include <cstddef>

#include "util/byte_buffer.h"
#include "container.h"

#define log_level oak::log::Level::NORMAL
#define log_print_out(t, ...) oak::log::cout.printf(t, log_level, __FILE__, __LINE__, ##__VA_ARGS__)
#define log_print_warn(t, ...) oak::log::cwarn.printf(t, log_level, __FILE__, __LINE__, ##__VA_ARGS__)
#define log_print_err(t, ...) oak::log::cerr.printf(t, log_level, __FILE__, __LINE__, ##__VA_ARGS__)

namespace oak::log {

	struct Stream {
		virtual void open() = 0;
		virtual void close() = 0;
		virtual void write(const void *source, size_t size) = 0;
		virtual void read(void *dest, size_t size) = 0;
	};

	enum class Level {
		MINIMAL,
		NORMAL,
		VERBOSE,
	};

	class Logger {
	public:
		Logger(const char* name);
		~Logger();

		void addStream(Stream *stream);

		template<typename... TArgs>
		void printf(const oak::string& text, Level level, const char* file, int line, TArgs&&... args) {
			char buffer[2048];
			sprintf(buffer, text.c_str(), std::forward<TArgs>(args)...);
			print(buffer, level, file, line);
		}

		void print(const char* text, Level level, const char* file, int line);
		void flush();

	private:
		char buffer_[2048];
		std::vector<Stream*> streams_;
		const char* name_;
	};

	extern Logger cout;
	extern Logger cwarn; 
	extern Logger cerr;

}