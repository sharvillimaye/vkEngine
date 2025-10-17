#include "yellowstone_window.hpp"

#include <stdexcept>

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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void YellowstoneWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void YellowstoneWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto yellowstoneWindow = reinterpret_cast<YellowstoneWindow*>(glfwGetWindowUserPointer(window));
		yellowstoneWindow->framebufferResized = true;
		yellowstoneWindow->width = width;
		yellowstoneWindow->height = height;
	}
}
