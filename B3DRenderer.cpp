#include "B3DRenderer.h"
#include <iostream>


B3DRenderer::B3DRenderer(B3DWindow &window, B3DDevice &device) : rendererWindow{window}, rendererDevice{device}
{
	recreateSwapChain();
	createCommandBuffers();
}

B3DRenderer::~B3DRenderer()
{
	freeCommandBuffers();
}

VkCommandBuffer B3DRenderer::beginFrame()
{
	assert(!isFrameStarted && "Can't begin frame while one is in progess!");

	auto result = rendererSwapChain->acquireNextImage(&currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to aquire next swap chain image!");
	}

	isFrameStarted = true;

	auto commandBuffer = getCurrentCommandBuffer();

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin command buffer recording!");
	}

	return commandBuffer;
}

void B3DRenderer::endFrame()
{
	assert(isFrameStarted && "Can't end a frame if none are stared!");

	auto commandBuffer = getCurrentCommandBuffer();

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to end command buffer recording!");
	}


	auto result = rendererSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rendererWindow.wasWindowResized())
	{
		rendererWindow.resetWindowResizedFlag();
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image!");
	}

	isFrameStarted = false;
	currentFrameIndex = (currentFrameIndex + 1) % B3DSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void B3DRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "Can't begin a render pass if no frames are started!");
	assert(commandBuffer == getCurrentCommandBuffer() && "Cannot perform a render pass on a different frame!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = rendererSwapChain->getRenderPass();
	renderPassInfo.framebuffer = rendererSwapChain->getFrameBuffer(currentImageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = rendererSwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(rendererSwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(rendererSwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{ {0, 0}, rendererSwapChain->getSwapChainExtent() };

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void B3DRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "Can't end a render pass if no frames are started!");
	assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end a render pass on a different frame!");

	vkCmdEndRenderPass(commandBuffer);
}

void B3DRenderer::createCommandBuffers()
{
	commandBuffers.resize(B3DSwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = rendererDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(rendererDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}
}

void B3DRenderer::freeCommandBuffers()
{
	vkFreeCommandBuffers(rendererDevice.device(), rendererDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	commandBuffers.clear();
}


void B3DRenderer::recreateSwapChain()
{
	auto extent = rendererWindow.getExtent();

	while (extent.width == 0 || extent.height == 0)
	{
		extent = rendererWindow.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(rendererDevice.device());

	if (rendererSwapChain == nullptr)
	{
		rendererSwapChain = std::make_unique<B3DSwapChain>(rendererDevice, extent);
	}
	else
	{
		std::shared_ptr<B3DSwapChain> oldSwapChain = std::move(rendererSwapChain);
		rendererSwapChain = std::make_unique<B3DSwapChain>(rendererDevice, extent, oldSwapChain);

		if (!oldSwapChain->compareSwapFormats(*rendererSwapChain.get()))
		{
			throw std::runtime_error("Swap chain image or depth format has changed!");
		}
	}
}