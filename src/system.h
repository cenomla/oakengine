#pragma once

namespace oak {

	class Engine;

	class System {
	public:
		System(Engine &engine);
		virtual ~System();

		virtual void init();
		virtual void destroy();

	protected:
		Engine &engine_;
	};


}