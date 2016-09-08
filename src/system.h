#pragma once

namespace oak {

	class System {
	public:
		System();
		virtual ~System();

		virtual void init();
		virtual void destroy();
	};


}