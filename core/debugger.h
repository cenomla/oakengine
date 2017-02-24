#pragma once

#include <unordered_map>
#include <string>
#include <chrono>

#include "system.h"
#include "pup.h"

namespace oak {

	struct PerformanceProfile {
		std::chrono::high_resolution_clock::time_point start;
		std::chrono::high_resolution_clock::time_point end;
		std::string name;
		size_t perfId;
		float durration;
		bool running;
	};

	struct DebugVars {
		float *dt = nullptr;
		float *fps = nullptr;
		size_t *usedMemory = nullptr;
		size_t *allocatedMemory = nullptr;
	};

	void pup(Puper& puper, DebugVars& data, const ObjInfo& info);

	class Debugger : public System {
	public:
		Debugger(Engine &engine);

		size_t createProfile(const std::string &name);
		void startProfile(size_t perfId);
		void endProfile(size_t perfId);
		const PerformanceProfile& getProfile(size_t perfId);

		DebugVars debugVars;
	private:
		std::vector<PerformanceProfile> profiles_;
	};

}