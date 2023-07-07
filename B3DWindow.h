#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>


class B3DWindow
{

	public:
		B3DWindow(int w, int h, std::string name);
		~B3DWindow();

		B3DWindow(const B3DWindow &) = delete;
		B3DWindow &operator=(const B3DWindow &) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		GLFWwindow* window;

		const int width;
		const int height;
		std::string windowName;

		void initWindow();
};