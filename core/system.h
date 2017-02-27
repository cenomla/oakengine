#pragma once

#include "memory/container.h"

namespace oak {

	class Engine;

	class System {
	public:
		virtual ~System();

		virtual void init();
		virtual void destroy();

		const oak::string& getName() const { return name_; }
	protected:
		System(Engine &engine, const oak::string &name);

		Engine &engine_;
		oak::string name_;
	};


}