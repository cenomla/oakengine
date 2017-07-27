#pragma once

#include "input.h"
#include "oak_assert.h"
#include "container.h"

struct GLFWwindow;

namespace oak {

	class InputManager {
	private:
		static InputManager *instance;
	public:
		inline static InputManager& inst() { 
			oak_assert(instance != nullptr);
			return *instance;
		}

		InputManager();
		~InputManager();

		void update();

		void bind(const oak::string& action, int binding, bool key);

		int getAction(const oak::string& action) const;
		void setAction(const oak::string& action, int state);

		int getKey(int key) const;
		void setKey(int key, int action);
		int getButton(int button) const;
		void getCursorPos(double *xpos, double *ypos) const;
		
		CursorMode getCursorMode() const;

		
		inline GLFWwindow* getWindow() const { return window_; }
	private:
		int actions_[1024];
		oak::unordered_map<oak::string, int> bindings_;
		GLFWwindow *window_;
	};

}