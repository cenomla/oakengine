#include "util.h"

namespace oak::math {

	float radians(float deg) {
		return deg / 180.0f * pi();
	}

	float pi() {
		return 3.14159265358979f;
	}

}
