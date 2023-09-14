#pragma once

#include "B3DDevice.h"

//Vulkan headers
#include <vulkan/vulkan.h>

//std headers
#include <memory>
#include <string>
#include <vector>

//Swap chain for Based 3D
class B3DSwapChain
{
	public:

		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		B3DSwapChain(B3DDevice &deviceRef, VkExtent2D windowExtent);
		~B3DSwapChain();

		B3DSwapChain(const B3DSwapChain&) = delete;
		void operator=(const B3DSwapChain&) = delete;

		VkFramebuffer getFrameBuffer(int index) { return swapChainFrameBuffers[index]; }
		VkRenderPass getRenderPass() { return renderPass; }
		VkImageView getImageView(int index) { return swapChainImageViews[index]; }
		size_t imageCount() { return swapChainImages.size(); }
		VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
		VkExtent2D getSwapChainExtent() { return swapChainExtent; }
		uint32_t width() { return swapChainExtent.width; }
		uint32_t hieght() { return swapChainExtent.height; }

		float extentAspectRatio() { return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height); }

		VkFormat findDepthFormat();

		VkResult acquireNextImage(uint32_t* imageIndex);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

	private:

		B3DDevice& device;

		
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		VkExtent2D windowExtent;

		std::vector<VkFramebuffer> swapChainFrameBuffers;
		VkRenderPass renderPass;

		std::vector<VkImage> depthImages;
		std::vector<VkDeviceMemory> depthImageMemorys;
		std::vector<VkImageView> depthImageViews;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		VkSwapchainKHR swapChain;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;

		size_t currentFrame = 0;

		void createSwapChain();
		void createImageViews();
		void createDepthResources();
		void createRenderPass();
		void createFrameBuffers();
		void createSyncObjects();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};