#include "api.h"

#include <fstream>
#include <GLFW/glfw3.h>

#include "graphics/vertex.h"

#include "log.h"

namespace oak::graphics {

	void OpenglApi::init(GLFWwindow *window) {
		//load opengl stuff
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		if (!gladLoadGL()) {
			log::cout << "cannot load gl" << std::endl;
			std::exit(-1);
		}

		log::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;
		window_ = window;

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glfwGetWindowSize(window_, &windowWidth_, &windowHeight_);

		setup();
	}

	void OpenglApi::destroy() {
		glDeleteBuffers(1, &vbo_);
		glDeleteBuffers(1, &ibo_);
		glDeleteVertexArrays(1, &vao_);
		glDeleteProgram(0);	
	}

	void OpenglApi::update() {
		glfwSwapBuffers(window_);
		glViewport(0, 0, windowWidth_, windowHeight_);
		glClearColor(0.3f, 0.2f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//bind shader and draw the triangle

		glUseProgram(pid_);
		glBindVertexArray(vao_);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0); 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glBindVertexArray(0);
		glUseProgram(0);

		//check for errors

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			log::cout << "opengl error: " << error << std::endl;
		}
	}

	void OpenglApi::onWindowResize(int width, int height) {
		windowWidth_ = width;
		windowHeight_ = height;
	}

	static std::string readFile(const std::string &path) {
		std::ifstream file{ path, std::ios::ate };

		if (!file.is_open()) {
			log::cout << "failed to open file: " << path << std::endl;
			std::exit(-1);
		}

		size_t len = file.tellg();
		std::vector<char> buffer(len);
		file.seekg(0);
		file.read(buffer.data(), len);

		file.close();

		return std::string{ buffer.data(), len };

	}

	GLuint createShader(const std::string &code, GLenum type) {
		GLuint shader = glCreateShader(type);

		const char *cstr = code.c_str();
		//compile shader
		glShaderSource(shader, 1, &cstr, NULL);
		glCompileShader(shader);
		//get and print results of compiliation
		GLint result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		//get log and print it
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length + 1);
		glGetShaderInfoLog(shader, length, &length, log.data());
		if (length > 1) {
			log::cout << log.data() << std::endl;
		}
		//exit of fail
		if (result == GL_FALSE) {
			log::cout << "failed to compile shader" << std::endl;
			std::exit(-1);
		}

		return shader;		
	}

	void OpenglApi::setup() {
		const auto vertCode = readFile("src/graphics/shaders/basic/opengl.vert");
		const auto fragCode = readFile("src/graphics/shaders/basic/opengl.frag");

		pid_ = glCreateProgram();
		GLuint vert = createShader(vertCode, GL_VERTEX_SHADER);
		GLuint frag = createShader(fragCode, GL_FRAGMENT_SHADER);
		glAttachShader(pid_, vert);
		glAttachShader(pid_, frag);
		glLinkProgram(pid_);
		glValidateProgram(pid_);
		glDeleteShader(vert);
		glDeleteShader(frag);

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Vertex::position)));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Vertex::color)));

		glBindBuffer(GL_ARRAY_BUFFER, vao_);

		glBindVertexArray(0);

		glGenBuffers(1, &ibo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}