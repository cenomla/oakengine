#pragma once

#include <iostream>

#ifdef _DEBUG

#define oak_assert(x) \
	do {\
		if (x) { std::cerr << "assert failed" << std::endl; std::exit(1); }\
	} while (0)

#else

#define oak_assert(x) (void)sizeof(x)

#endif //_DEBUG