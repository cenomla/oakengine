#pragma once

#include <glad/glad.h>

struct GLFWwindow;

namespace oak::graphics {

	class OpenglApi {
	public:
		void init(GLFWwindow *window);
		void destroy();

		void update();

		void onWindowResize(int width, int height);

	private:
		GLFWwindow *window_;

		GLuint vao_;
		GLuint vbo_;
		GLuint ibo_;
		GLuint pid_;

		int windowWidth_, windowHeight_;

		void setup();

	};

}