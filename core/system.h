#pragma once

#include <string>

namespace oak {

	class Engine;

	class System {
	public:
		System(Engine &engine, const std::string &name);
		virtual ~System();

		virtual void init();
		virtual void destroy();

		const std::string& getName() const { return name_; }
	protected:
		Engine &engine_;
		std::string name_;
	};


}