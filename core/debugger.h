#pragma once

#include <chrono>

#include "container.h"

namespace oak {

	struct PerformanceProfile {
		std::chrono::high_resolution_clock::time_point start;
		std::chrono::high_resolution_clock::time_point end;
		oak::string name;
		size_t perfId;
		float durration;
		bool running;
	};

	class Debugger {
	public:
		size_t createProfile(const oak::string &name);
		void startProfile(size_t perfId);
		void endProfile(size_t perfId);
		const PerformanceProfile& getProfile(size_t perfId);
	private:
		oak::vector<PerformanceProfile> profiles_;
	};

}