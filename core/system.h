#pragma once

namespace oak {

	class System {
	public:
		virtual ~System();

		virtual void init();
		virtual void terminate();

		virtual void run();
	};


}