#pragma once

#include <unordered_map>
#include <string>
#include <chrono>

namespace oak {

	struct PerformanceProfile {
		std::chrono::high_resolution_clock::time_point start;
		std::chrono::high_resolution_clock::time_point end;
		std::chrono::high_resolution_clock::duration totalTime;
		bool running;
	};

	class Debugger {
	public:

		void startProfile(const std::string &name);
		void endProfile(const std::string &name);
		const PerformanceProfile& getProfile(const std::string &name);

	private:
		std::unordered_map<std::string, PerformanceProfile> profiles_;
	};

}