#pragma once

#include <iostream>
#include <fstream>

#include "container.h"
#include "log.h"

namespace oak::util {

	inline oak::vector<char> readFile(const char *path) {
		std::ifstream file{ path, std::ios::ate | std::ios::binary };

		if (!file.is_open()) {
			log_print_err("couldnt open file: %s", path);
			abort();
		}

		auto size = file.tellg();
		oak::vector<char> buffer(size);
		file.seekg(0);
		file.read(buffer.data(), size);

		file.close();

		return buffer;
	}

	inline oak::string readFileAsString(const char *path) {
		std::ifstream file{ path, std::ios::binary | std::ios::ate };

		if (!file.is_open()) {
			log_print_err("couldnt open file: %s", path);
			abort();
		}

		auto size = file.tellg();
		oak::string buffer(size, '\0');
		file.seekg(0);
		file.read(&buffer[0], size);

		file.close();

		return buffer;
	}

}