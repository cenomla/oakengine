#pragma once


#include <chrono>

#include "debug_vars.h"
#include "container.h"
#include "system.h"
#include "pup.h"

namespace oak {

	struct PerformanceProfile {
		std::chrono::high_resolution_clock::time_point start;
		std::chrono::high_resolution_clock::time_point end;
		oak::string name;
		size_t perfId;
		float durration;
		bool running;
	};

	void pup(Puper& puper, DebugVars& data, const ObjInfo& info);

	class Debugger : public System {
	public:
		Debugger(Engine &engine);

		size_t createProfile(const oak::string &name);
		void startProfile(size_t perfId);
		void endProfile(size_t perfId);
		const PerformanceProfile& getProfile(size_t perfId);
	private:
		oak::vector<PerformanceProfile> profiles_;
	};

}