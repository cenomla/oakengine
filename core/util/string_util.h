#pragma once

#include "memory/container.h"

namespace oak::util {

	inline void splitstr(const oak::string& str, const oak::string& delimeters, oak::vector<oak::string>& tokens) {
		size_t prev = 0, pos;
		while ((pos = str.find_first_of(delimeters, prev)) != oak::string::npos) {
			if (pos > prev) {
				tokens.push_back(str.substr(prev, pos-prev));
			}
			prev = pos + 1;
		}
		if (prev < str.length()) {
			tokens.push_back(str.substr(prev, oak::string::npos));
		}
	}

}