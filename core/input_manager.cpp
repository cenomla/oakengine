#include "input_manager.h"

#include <GLFW/glfw3.h>

#include "input.h"
#include "input_events.h"
#include "event_manager.h"
#include "log.h"

namespace oak {

	void setCallbacks(GLFWwindow *window);

	InputManager *InputManager::instance = nullptr;

	InputManager::InputManager() {
		oak_assert(instance == nullptr);
		instance = this;

		if (!glfwInit()) {
			log_print_err("failed to initialize glfw");
			abort();
		}
	}
	
	InputManager::~InputManager() {
		instance = nullptr;
		glfwTerminate();
	}

	void InputManager::update() {
		for (const auto& evt : EventManager::inst().getQueue<WindowCreateEvent>()) {
			setCallbacks(evt.window);
		}
		glfwPollEvents();
		for (const auto& evt : EventManager::inst().getQueue<KeyEvent>()) {
			actions_[evt.key + button::max] = evt.action;
		}

		for (const auto& evt : EventManager::inst().getQueue<ButtonEvent>()) {
			actions_[evt.button] = evt.action;
		}
	}

	void InputManager::bind(const oak::string& action, int binding, bool key) {
		bindings_[action] = binding + (key ? button::max : 0);
	}

	int InputManager::getAction(const oak::string& action) const {
		const auto& it = bindings_.find(action);
		if (it != std::end(bindings_)) {
			return actions_[it->second];
		} else {
			log_print_out("action not bound: %s", action.c_str());
			return 0;
		}
	}

	int InputManager::getKey(int key) const {
		return actions_[key + button::max];
	}

	int InputManager::getButton(int button) const {
		return actions_[button];
	}
	static void closeCallback(GLFWwindow *window) {
		EventManager::inst().getQueue<WindowCloseEvent>().emit({ window });
	}

	static void resizeCallback(GLFWwindow *window, int width, int height) {
		EventManager::inst().getQueue<WindowResizeEvent>().emit({ width, height });
	}

	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		EventManager::inst().getQueue<KeyEvent>().emit({ key, scancode, action, mods });
	}

	static void buttonCallback(GLFWwindow *window, int button, int action, int mods) {
		EventManager::inst().getQueue<ButtonEvent>().emit({ button, action, mods });
	}

	static void cursorCallback(GLFWwindow *window, double xpos, double ypos) {
		EventManager::inst().getQueue<CursorEvent>().emit({ static_cast<float>(xpos), static_cast<float>(ypos) });
	}

	static void charCallback(GLFWwindow *window, uint32_t codepoint) {
		EventManager::inst().getQueue<TextEvent>().emit(TextEvent{ codepoint });
	}

	void setCallbacks(GLFWwindow *window) {
		glfwSetWindowCloseCallback(window, closeCallback);
		glfwSetWindowSizeCallback(window, resizeCallback);
		glfwSetKeyCallback(window, keyCallback);
		glfwSetMouseButtonCallback(window, buttonCallback);
		glfwSetCursorPosCallback(window, cursorCallback);
		glfwSetCharCallback(window, charCallback);
	}

}