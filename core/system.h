#pragma once

#include <string>

namespace oak {

	class Engine;

	class System {
	public:
		virtual ~System();

		virtual void init();
		virtual void destroy();

		const std::string& getName() const { return name_; }
	protected:
		System(Engine &engine, const std::string &name);

		Engine &engine_;
		std::string name_;
	};


}