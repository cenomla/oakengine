#include "input_manager.h"

#include <GLFW/glfw3.h>

#include "input.h"
#include "input_events.h"
#include "oakengine.h"
#include "log.h"

namespace oak {

	void setCallbacks(GLFWwindow *window);

	InputManager *InputManager::instance = nullptr;

	InputManager::InputManager() : actions_{ 0 }, window_{ nullptr } {
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

	void InputManager::updateCache() {
		for (const auto& evt : getEventQueue<WindowCreateEvent>()) {
			window_ = evt.window;
			setCallbacks(evt.window);
		}
		for (const auto& evt : getEventQueue<KeyEvent>()) {
			actions_[evt.key + button::max] = evt.action;
		}

		for (const auto& evt : getEventQueue<ButtonEvent>()) {
			actions_[evt.button] = evt.action;
		}
		for (const auto& evt : getEventQueue<CursorModeEvent>()) {
			if (!window_) { break; }
			switch (evt.mode) {
				case CursorMode::NORMAL:
					glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					break;
				case CursorMode::HIDDEN:
					glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
					break;
				case CursorMode::DISABLED:
					glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					break;
			}
		}
	}

	void InputManager::poll() {
		glfwPollEvents();
	}

	void InputManager::bind(const oak::string& action, int binding, bool key) {
		bindings_[action] = binding + (key ? button::max : 0);
	}

	int InputManager::getAction(const oak::string& action) const {
		const auto& it = bindings_.find(action);
		if (it != std::end(bindings_)) {
			return actions_[it->second];
		} else {
			log_print_warn("action not bound: %s", action.c_str());
			return 0;
		}
	}

	int InputManager::getActionIndex(const oak::string& action) const {
		const auto& it = bindings_.find(action);
		if (it != std::end(bindings_)) {
			return it->second - (it->second > button::max ? button::max : 0);
		} else {
			log_print_warn("action not bound: %s", action.c_str());
			return 0;
		}
	}

	void InputManager::setAction(const oak::string& action, int state) {
		const auto& it = bindings_.find(action);
		if (it != std::end(bindings_)) {
			actions_[it->second] = state;
		} else {
			log_print_warn("action not bound: %s", action.c_str());
		}
	}

	int InputManager::getKey(int key) const {
		return actions_[key + button::max];
	}

	void InputManager::setKey(int key, int action) {
		actions_[key + button::max] = action;
	}

	int InputManager::getButton(int button) const {
		return actions_[button];
	}

	void InputManager::setButton(int button, int action) {
		actions_[button] = action;
	}

	glm::vec2 InputManager::getCursorPos() const {
		if (!window_) {
			return { 0, 0 };
		}
		double x, y;
		glfwGetCursorPos(window_, &x, &y);
		return { x, y };
	}

	glm::ivec2 InputManager::getWindowSize() const {
		if (!window_) {
			return { 0, 0 };
		}
		glm::ivec2 size;
		glfwGetWindowSize(window_, &size.x, &size.y);
		return size;
	}

	CursorMode InputManager::getCursorMode() const {
		if (window_) {
			int mode = glfwGetInputMode(window_, GLFW_CURSOR);
			switch (mode) {
				case GLFW_CURSOR_NORMAL: return CursorMode::NORMAL;
				case GLFW_CURSOR_HIDDEN: return CursorMode::HIDDEN;
				case GLFW_CURSOR_DISABLED: return CursorMode::DISABLED;
			}
		}
		return CursorMode::NORMAL;
	}

	static void closeCallback(GLFWwindow *window) {
		emitEvent<WindowCloseEvent>(window);
	}

	static void resizeCallback(GLFWwindow *window, int width, int height) {
		emitEvent<WindowResizeEvent>(width, height);
	}

	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		emitEvent<KeyEvent>(key, scancode, action, mods);
	}

	static void buttonCallback(GLFWwindow *window, int button, int action, int mods) {
		emitEvent<ButtonEvent>(button, action, mods);
	}

	static void cursorCallback(GLFWwindow *window, double xpos, double ypos) {
		emitEvent<CursorEvent>(static_cast<float>(xpos), static_cast<float>(ypos));
	}

	static void charCallback(GLFWwindow *window, uint32_t codepoint) {
		emitEvent<TextEvent>(codepoint);
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
