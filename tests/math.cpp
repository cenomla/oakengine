#include <oakengine.h>
#include <math/mat.h>

int main(int argc, char **argv) {

	oak::math::Mat4 mat{ 2 };

	oak::math::Vec4 vec{ 2 };

	auto r = mat * vec;

	if (r != oak::math::Vec4{ 4 }) {
		return -1;
	}

	return 0;

}
