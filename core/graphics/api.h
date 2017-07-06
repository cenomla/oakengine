#pragma once

namespace oak::graphics {

	class Api {
	public:

		virtual void init() = 0;
		virtual void terminate() = 0;
		
		virtual void swap() = 0;
	};

}