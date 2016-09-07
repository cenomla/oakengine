#pragma once

#include <functional>
#include <bitset>

namespace oak {

	struct Task {
		static constexpr size_t LOOP = 0;
		static constexpr size_t MULTI_THREAD = 1;
		static constexpr size_t BACKGROUND = 2;
		static constexpr size_t LOOP_BIT = 1;
		static constexpr size_t MULTI_THREAD_BIT = 2;
		static constexpr size_t BACKGROUND_BIT = 6;

		std::function<void ()> run;
		std::bitset<3> flags;
	};

}