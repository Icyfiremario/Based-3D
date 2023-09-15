#include "Game.h"
#include <iostream>



Game::Game()
{
	createPipelineLayout();
	createPipeline();
	createCommandBuffers();
}

Game::~Game()
{
	vkDestroyPipelineLayout(gameDevice.device(), pipelineLayout, nullptr);
}

void Game::run()
{
	while (!gameWindow.shouldClose())
	{
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(gameDevice.device());
}

void Game::createPipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(gameDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void Game::createPipeline()
{
	PipelineConfigInfo pipelineConfig{};

	B3DPipeline::deafultPipelineConfigInfo(pipelineConfig, gameSwapChain.width(), gameSwapChain.hieght());

	pipelineConfig.renderPass = gameSwapChain.getRenderPass();
	pipelineConfig.pipelineLayout = pipelineLayout;

	gameRenderPipeline = std::make_unique<B3DPipeline>(gameDevice, "simple_shader.vert.spv", "simple_shader.frag.spv", pipelineConfig);
}

void Game::createCommandBuffers()
{
	commandBuffers.resize(gameSwapChain.imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = gameDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(gameDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	for (int i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin command buffer recording!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = gameSwapChain.getRenderPass();
		renderPassInfo.framebuffer = gameSwapChain.getFrameBuffer(i);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = gameSwapChain.getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		gameRenderPipeline->bind(commandBuffers[i]);

		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end command buffer recording!");
		}
	}
}

void Game::drawFrame()
{
	uint32_t imageIndex;

	auto result = gameSwapChain.acquireNextImage(&imageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to aquire next swap chain image!");
	}

	result = gameSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image!");
	}
}