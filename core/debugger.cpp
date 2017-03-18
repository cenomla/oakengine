#include "debugger.h"

#include "log.h"

namespace oak {

	DebugVars debugVars;

	void pup(Puper& puper, DebugVars& data, const ObjInfo& info) {
		pup(puper, data.dt, info + ObjInfo{ "delta_time" });
		pup(puper, data.fps, info + ObjInfo{ "fps" });
		pup(puper, data.usedMemory, info + ObjInfo{ "used_memory" });
		pup(puper, data.allocatedMemory, info + ObjInfo{ "allocated_memory" });
	}

	Debugger::Debugger(Engine &engine) : System{ engine, "debugger" } {}

	size_t Debugger::createProfile(const oak::string& name) {
		size_t id = profiles_.size();

		PerformanceProfile prof;
		prof.name = name;
		prof.perfId = id;
		prof.running = false;

		profiles_.resize(id + 1);
		profiles_.push_back(std::move(prof));

		return id;
	}

	void Debugger::startProfile(size_t perfId) {
		auto& it = profiles_[perfId];
		
		it.start = std::chrono::high_resolution_clock::now();
		it.running = true;
	}

	void Debugger::endProfile(size_t perfId) {
		auto& it = profiles_[perfId];

		it.end = std::chrono::high_resolution_clock::now();
		it.running = false;
		it.durration = std::chrono::high_resolution_clock::duration{ it.end - it.start }.count();
	}

	const PerformanceProfile& Debugger::getProfile(size_t perfId) {
		return profiles_.at(perfId);
	}

}