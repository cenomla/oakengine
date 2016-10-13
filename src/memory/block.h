#pragma once

#include <iostream>

namespace oak {

	typedef unsigned char byte;

	struct Block {
		void *ptr = nullptr;
		size_t size = 0;
	};

}