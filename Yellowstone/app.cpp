#include "app.hpp"

namespace yellowstone {
	void App::run() {
		while (!yellowstoneWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
}