#include "B3DSwapChain.h"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>


B3DSwapChain::B3DSwapChain(B3DDevice& deviceRef, VkExtent2D extent) : device{deviceRef}, windowExtent{extent}
{
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDepthResources();
	createFrameBuffers();
	createSyncObjects();
}

B3DSwapChain::~B3DSwapChain()
{
	for (auto imageView : swapChainImageViews)
	{
		vkDestroyImageView(device.device(), imageView, nullptr);
	}
	swapChainImageViews.clear();

	if (swapChain != nullptr)
	{
		vkDestroySwapchainKHR(device.device(), swapChain, nullptr);
		swapChain = nullptr;
	}

	for (int i = 0; i < depthImages.size(); i++)
	{
		vkDestroyImageView(device.device(), depthImageViews[i], nullptr);
		vkDestroyImage(device.device(), depthImages[i], nullptr);
		vkFreeMemory(device.device(), depthImageMemorys[i], nullptr);
	}

	for (auto frameBuffer : swapChainFrameBuffers)
	{
		vkDestroyFramebuffer(device.device(), frameBuffer, nullptr);
	}

	vkDestroyRenderPass(device.device(), renderPass, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device.device(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device.device(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device.device(), inFlightFences[i], nullptr);
	}
}

VkFormat B3DSwapChain::findDepthFormat()
{
	return VkFormat();
}

VkResult B3DSwapChain::acquireNextImage(uint32_t* imageIndex)
{
	vkWaitForFences(device.device(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	VkResult result = vkAcquireNextImageKHR(device.device(), swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, imageIndex);

	return result;
}

VkResult B3DSwapChain::submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex)
{
	if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(device.device(), 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
	}

	imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = buffers;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device.device(), 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = imageIndex;

	auto result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	return result;
}

void B3DSwapChain::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = device.surface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyInices indices = device.findPhysicalQueueFamilies();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device.device(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void B3DSwapChain::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	}
}

void B3DSwapChain::createDepthResources()
{
}

void B3DSwapChain::createRenderPass()
{
	VkAttachmentDescription depthAttachment{};
}

void B3DSwapChain::createFrameBuffers()
{
}

void B3DSwapChain::createSyncObjects()
{
}

VkSurfaceFormatKHR B3DSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	return VkSurfaceFormatKHR();
}

VkPresentModeKHR B3DSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	return VkPresentModeKHR();
}


VkExtent2D B3DSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = windowExtent;
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
	return VkExtent2D();
}
