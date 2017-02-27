#include "debugger.h"

#include "log.h"

namespace oak {

	DebugVars debugVars;

	void pup(Puper& puper, DebugVars& data, const ObjInfo& info) {
		pup(puper, data.dt, ObjInfo{ "delta_time" } + info);
		pup(puper, data.fps, ObjInfo{ "fps" } + info);
		pup(puper, data.usedMemory, ObjInfo{ "used_memory" } + info);
		pup(puper, data.allocatedMemory, ObjInfo{ "allocated_memory" } + info);
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