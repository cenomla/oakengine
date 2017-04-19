#pragma once

#include <cinttypes>

#include "container.h"
#include "system.h"

struct GLFWwindow;

namespace oak {


	class WindowSystem : public System {
	public:

		WindowSystem(const oak::string& title);

		void init() override;
		void terminate() override;

		void run() override;

	private:
		GLFWwindow *window_;
		oak::string title_;

		void createWindow();
		void setCallbacks();

	};

}
