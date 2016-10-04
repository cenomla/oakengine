#pragma once

#include <glad/glad.h>

#include "gltexture.h"
#include "glshader.h"
#include "glvertex_array.h"
#include "glbuffer.h"

struct GLFWwindow;

namespace oak::graphics {

	class OpenglApi {
	public:
		OpenglApi();

		void init(GLFWwindow *window);
		void destroy();

		void update();

		void onWindowResize(int width, int height);

	private:
		GLFWwindow *window_;
		int windowWidth_, windowHeight_;
	};

}