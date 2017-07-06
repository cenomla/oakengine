#pragma once

#include "graphics/api.h"


struct GLFWwindow;

namespace oak::graphics {

	class GLApi : public Api {
	public:

		void init() override;
		void terminate() override;

		void swap() override;

	private:
		GLFWwindow *window_;
	};

}