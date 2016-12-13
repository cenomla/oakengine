#pragma once

#include <iostream>

#ifdef _DEBUG

#define oak_assert(x) \
	do {\
		if (!(x)) { std::cout << "assert failed: " << __FILE__ << ", " << __LINE__ << std::endl; std::exit(1); }\
	} while (0)

#else

#define oak_assert(x) (void)sizeof(x)

#endif //_DEBUG