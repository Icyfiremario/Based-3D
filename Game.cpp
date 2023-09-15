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
}

void Game::drawFrame()
{
}
