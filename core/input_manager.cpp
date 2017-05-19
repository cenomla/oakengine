#include "input_manager.h"

#include "input.h"
#include "input_events.h"
#include "event_manager.h"
#include "log.h"

namespace oak {

	InputManager *InputManager::instance = nullptr;

	InputManager::InputManager() {
		oak_assert(instance == nullptr);
		instance = this;
	}
	
	InputManager::~InputManager() {
		instance = nullptr;
	}

	void InputManager::update() {
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

}