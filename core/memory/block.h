#pragma once

#include <iostream>

namespace oak {

	struct Block {
		void *ptr = nullptr;
		size_t size = 0;
	};

	template<class T> 
	struct TBlock {
		T *ptr = nullptr;
		size_t size = 0;
	};

}