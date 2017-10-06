#pragma once

#include <cstring>

namespace oak {

	template<class T>
	struct Array;

	struct String {
		static constexpr size_t npos = 0xFFFFFFFFFFFFFFFF;

		String substr(size_t start, size_t end);
		size_t find_first_of(const String& delimeters, size_t start);
		void splitstr(const String& delimeters, Array<String>& tokens);

		const char *data = nullptr;
		size_t length = 0;
	};

	inline operator String(const char *str) { return String{ str, strlen(str) }; }
}

