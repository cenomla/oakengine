#pragma once


struct GLFWwindow;

namespace oak::graphics {

	class OpenglApi {
	public:
		void init(GLFWwindow *window);
		void destroy();

		void update();

	private:
		GLFWwindow *window_;

	};

}