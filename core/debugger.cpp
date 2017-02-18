#include "debugger.h"

#include "log.h"

namespace oak {

	Debugger::Debugger(Engine &engine) : System{ engine, "debugger" } {}

	size_t Debugger::createProfile(const std::string& name) {
		size_t id = std::hash<std::string>{}(name);
		const auto& it = profiles_.find(id);

		if (it == std::end(profiles_)) {
			const auto& it = profiles_.insert({ id, { } });
			it.first->second.name = name;
			it.first->second.perfId = id;
			it.first->second.running = false;
		}

		return id;
	}

	void Debugger::startProfile(size_t perfId) {
		const auto& it = profiles_.find(perfId);

		if (it != std::end(profiles_)) {
			it->second.start = std::chrono::high_resolution_clock::now();
			it->second.running = true;
		}
	}

	void Debugger::endProfile(size_t perfId) {
		const auto& it = profiles_.find(perfId);

		if (it != std::end(profiles_)) {
			it->second.end = std::chrono::high_resolution_clock::now();
			it->second.running = false;
			it->second.totalTime += it->second.end - it->second.start;
		}
	}

	const PerformanceProfile& Debugger::getProfile(size_t perfId) {
		const auto& it = profiles_.find(perfId);

		if (it != std::end(profiles_)) {
			return it->second;
		}

		log::cout << "profile doesnt exist" << std::endl;
		abort();
	}

	const PerformanceProfile& Debugger::getProfile(const std::string &name) {
		return getProfile(std::hash<std::string>{}(name));
	}

}