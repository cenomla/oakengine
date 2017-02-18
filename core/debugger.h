#pragma once

#include <unordered_map>
#include <string>
#include <chrono>

#include "system.h"

namespace oak {

	struct PerformanceProfile {
		std::chrono::high_resolution_clock::time_point start;
		std::chrono::high_resolution_clock::time_point end;
		std::chrono::high_resolution_clock::duration totalTime;
		std::string name;
		size_t perfId;
		bool running;
	};

	struct VariableView {
		void *data;

		size_t getSizeT() { return *static_cast<size_t*>(data); }
		int getInt() { return *static_cast<int*>(data); }
		float getFloat() { return *static_cast<float*>(data); }
	};

	class Debugger : public System {
	public:
		Debugger(Engine &engine);

		size_t createProfile(const std::string &name);
		void startProfile(size_t perfId);
		void endProfile(size_t perfId);

		const PerformanceProfile& getProfile(size_t perfId);
		const PerformanceProfile& getProfile(const std::string &name);

	private:
		std::unordered_map<size_t, PerformanceProfile> profiles_;
		std::unordered_map<std::string, VariableView> views_;
	};

}