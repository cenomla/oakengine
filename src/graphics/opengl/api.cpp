#include "api.h"

#include <chrono>
#include <fstream>
#include <GLFW/glfw3.h>

#define STBI_IMPLEMENTATION
#define STBI_PNG_ONLY
#include <stb_image.h>

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

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//update matrices
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4{ 1.0f }, time * glm::radians(90.0f), glm::vec3{ 0.0f, 0.0f, 1.0f });
		ubo.view = glm::lookAt(glm::vec3{ 2.0f, 2.0f, 2.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f });
		ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(windowWidth_) / static_cast<float>(windowWidth_), 0.1f, 10.0f);

		//ubo.proj[1][1] *= -1;
		//TODO: device whether or not to flip the y coord ubo.proj[1][1] *= -1;

		glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo), &ubo, GL_STREAM_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//bind shader and draw the triangle

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_);
		glUseProgram(pid_);
		glBindVertexArray(vao_);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0); 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

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

		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Vertex::position)));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Vertex::color)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Vertex::texCoords)));

		glBindBuffer(GL_ARRAY_BUFFER, vao_);

		glBindVertexArray(0);

		glGenBuffers(1, &ibo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//load texture
		int w, h, comp;
		stbi_uc *data = stbi_load("res/textures/tex.png", &w, &h, &comp, 4);

		if (data == nullptr) {
			log::cout << "failed to load texture" << std::endl;
			std::exit(-1);
		}

		glGenTextures(1, &tex_);
		glBindTexture(GL_TEXTURE_2D, tex_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);

		//setup uniform buffer
		GLuint blockIndex = glGetUniformBlockIndex(pid_, "MatrixBlock");
		glUniformBlockBinding(pid_, blockIndex, 0);

		glGenBuffers(1, &ubo_);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_);

	}

}