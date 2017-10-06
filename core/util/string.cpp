#include "string.h"

#include "array.h"

namespace oak {

	String String::substr(const String& str, size_t start, size_t end) {
		return String{ str.data + start, end - start }; 
	}

	size_t String::find_first_of(const String& str, const String& delimeters, size_t start) {
		for (size_t i = start; i < str.length; i++) {
			for (size_t j = 0; j < delimeters.length; j++) {
				if (str.data[i] == delimeters.data[i]) {
					return i;
				}
			}
		}
		return String::npos;
	}

	void String::splitstr(const String& str, const String& delimeters, Array<String>& tokens) {
		size_t prev = 0, pos;
		do {
			pos = find_first_of(str, delimeters, prev);
			if (pos > prev) {
				tokens.push(substr(str, prev, pos-prev));
			}
			prev = pos + 1;
		} while(pos != String::npos);
	}

}
