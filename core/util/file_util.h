#pragma once

#include <iostream>
#include <fstream>

#include "log.h"

namespace oak::util {

	inline std::vector<char> readFile(const std::string &path) {
		std::ifstream file{ path, std::ios::ate | std::ios::binary };

		if (!file.is_open()) {
			log::cout << "couldnt open file: " << path << std::endl;
			abort();
		}

		auto size = file.tellg();
		std::vector<char> buffer(size);
		file.seekg(0);
		file.read(buffer.data(), size);

		file.close();

		return buffer;
	}

	inline std::string readFileAsString(const std::string &path) {
		std::ifstream file{ path, std::ios::binary | std::ios::ate };

		if (!file.is_open()) {
			log::cout << "couldnt open file: " << path << std::endl;
			abort();
		}

		auto size = file.tellg();
		std::string buffer(size, '\0');
		file.seekg(0);
		file.read(&buffer[0], size);

		file.close();

		return buffer;
	}

}