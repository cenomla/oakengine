#pragma once

#include <iostream>

namespace oak {

	typedef unsigned char byte;

	struct Block {
		void *ptr;
		size_t size;
	};

}