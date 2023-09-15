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
		bool wasWindowResized() { return frameBufferResized; }

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width),  static_cast<uint32_t>(height) }; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		void resetWindowResizedFlag() { frameBufferResized = false; }

	private:
		GLFWwindow* window;

		int width;
		int height;

		bool frameBufferResized = false;

		std::string windowName;

		void initWindow();

		static void framebufferResizedCallback(GLFWwindow* window, int width, int height);
};