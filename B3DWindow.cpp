#include "B3DWindow.h"


B3DWindow::B3DWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}
{
	initWindow();
}

B3DWindow::~B3DWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void B3DWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}
}

void B3DWindow::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
}

void B3DWindow::framebufferResizedCallback(GLFWwindow* window, int width, int height)
{
	auto appWindow = reinterpret_cast<B3DWindow*>(glfwGetWindowUserPointer(window));

	appWindow->frameBufferResized = true;
	appWindow->width = width;
	appWindow->height = height;
}
