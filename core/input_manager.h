#pragma once

#include "oak_assert.h"

#include "container.h"

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
		int getKey(int key) const;
		int getButton(int button) const;
	private:
		int actions_[1024];
		oak::unordered_map<oak::string, int> bindings_;
	};

}