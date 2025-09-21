#include "yellowstone_window.hpp"

namespace yellowstone {
	YellowstoneWindow::YellowstoneWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	YellowstoneWindow::~YellowstoneWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void YellowstoneWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}
}
