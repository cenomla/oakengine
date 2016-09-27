#pragma once

#include <iostream>
#include <fstream>

#include "log.h"

namespace oak::util {

	inline std::vector<char> readFile(const std::string &path) {
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			log::cout << "couldnt open file: " << path << std::endl;
			std::exit(-1);
		}

		size_t size = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(size);
		file.seekg(0);
		file.read(buffer.data(), size);

		file.close();

		return buffer;
	}

	inline std::string readFileAsString(const std::string &path) {

		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			log::cout << "couldnt open file: " << path << std::endl;
			std::exit(-1);
		}

		size_t size = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(size);
		file.seekg(0);
		file.read(buffer.data(), size);

		file.close();

		return std::string{ buffer.data(), buffer.size() };
	}

}