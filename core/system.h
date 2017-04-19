#pragma once

namespace oak {

	class System {
	public:
		virtual void init();
		virtual void terminate();

		virtual void run();
	};


}