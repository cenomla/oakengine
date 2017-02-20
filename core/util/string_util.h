#pragma once

#include <string>
#include <vector>

namespace oak::util {

	inline void splitstr(const std::string& str, const std::string& delimeters, std::vector<std::string>& tokens) {
		size_t prev = 0, pos;
		while ((pos = str.find_first_of(delimeters, prev)) != std::string::npos) {
			if (pos > prev) {
				tokens.push_back(str.substr(prev, pos-prev));
			}
			prev = pos + 1;
		}
		if (prev < str.length()) {
			tokens.push_back(str.substr(prev, std::string::npos));
		}
	}

}