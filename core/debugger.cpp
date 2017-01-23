#include "debugger.h"

#include "log.h"

namespace oak {

	void Debugger::startProfile(const std::string &name) {
		const auto it = profiles_.find(name);

		if (it != std::end(profiles_)) {
			it->second.start = std::chrono::high_resolution_clock::now();
			it->second.running = true;
		} else {
			auto it = profiles_.insert({ name, {} });
			it.first->second.start = std::chrono::high_resolution_clock::now();
			it.first->second.running = true;
		}
	}

	void Debugger::endProfile(const std::string &name) {
		const auto it = profiles_.find(name);

		if (it != std::end(profiles_)) {
			it->second.end = std::chrono::high_resolution_clock::now();
			it->second.running = false;
			it->second.totalTime += it->second.end - it->second.start;
		}
	}

	const PerformanceProfile& Debugger::getProfile(const std::string &name) {
		const auto it = profiles_.find(name);

		if (it != std::end(profiles_)) {
			return it->second;
		}

		log::cout << "profile doesnt exist" << std::endl;
		abort();
	}

}