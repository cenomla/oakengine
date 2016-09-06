#pragma once

#include <bitset>

namespace oak {

	struct Task {
		static constexpr size_t LOOP = 0b0001;
		static constexpr size_t MAIN_THREAD = 0b0010;
		static constexpr size_t MULTI_THREAD = 0b0100;

		Task(int f) : flags{ f } {}
		virtual ~Task() {}
		virtual void run() = 0;

		std::bitset<3> flags;
	};

}