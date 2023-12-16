#pragma once

//Local
#include "B3DDevice.h"
#include "B3DSwapChain.h"
#include "B3DWindow.h"

//STD
#include <cassert>
#include <memory>
#include <vector>

class B3DRenderer
{
	public:

		B3DRenderer(B3DWindow &window, B3DDevice &device);
		~B3DRenderer();

		B3DRenderer(const B3DRenderer&) = delete;
		B3DRenderer& operator=(const B3DRenderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		bool isFrameInProgress() const { return isFrameStarted; }

		VkRenderPass getSwapChainRenderPass() const { return rendererSwapChain->getRenderPass(); }

		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentImageIndex];
		}

	private:

		B3DWindow &rendererWindow;
		B3DDevice &rendererDevice;
		std::unique_ptr<B3DSwapChain> rendererSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		bool isFrameStarted = false;

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
};