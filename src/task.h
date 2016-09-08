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

		Task(const std::function<void ()> &r, const std::bitset<3> f) : run{ r }, flags{ f } {}
		Task(const Task &other) : run{ other.run }, flags{ other.flags } {}
		Task(Task &&other) : run{ std::move(other.run) }, flags{ std::move(other.flags) } {}

		std::function<void ()> run;
		std::bitset<3> flags;
	};

}