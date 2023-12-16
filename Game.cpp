#include "Game.h"
#include <iostream>

struct SimplePushConstantData
{
	glm::mat2 transform{ 1.f };
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

Game::Game()
{
	loadGameObjects();
	createPipelineLayout();
	recreateSwapChain();
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
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(gameDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void Game::createPipeline()
{
	assert(gameSwapChain != nullptr && "Cannot create pipeline before swap chain");
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};

	B3DPipeline::deafultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = gameSwapChain->getRenderPass();
	pipelineConfig.pipelineLayout = pipelineLayout;

	gameRenderPipeline = std::make_unique<B3DPipeline>(gameDevice, "simple_shader.vert.spv", "simple_shader.frag.spv", pipelineConfig);
}

void Game::createCommandBuffers()
{
	commandBuffers.resize(gameSwapChain->imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = gameDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(gameDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	
}

void Game::drawFrame()
{
	uint32_t imageIndex;

	auto result = gameSwapChain->acquireNextImage(&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to aquire next swap chain image!");
	}

	recordCommandBuffer(imageIndex);
	result = gameSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || gameWindow.wasWindowResized())
	{
		gameWindow.resetWindowResizedFlag();
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image!");
	}
}

void Game::freeCommandBuffers()
{
	vkFreeCommandBuffers(gameDevice.device(), gameDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	commandBuffers.clear();
}

void Game::loadGameObjects()
{
	std::vector<B3DModel::Vertex> vertices{
		{{ -0.5f, 0.5f }, {0.03, 0.49, 0.22} },
		{{ 0.5f, 0.5f }, {0.03, 0.49, 0.22}},
		{{ 0.0f, -0.5f }, {0.03, 0.49, 0.22}}
	};

	auto gameModel = std::make_shared<B3DModel>(gameDevice, vertices);
	auto triangle = B3DGameObj::createGameObject();
	triangle.model = gameModel;
	triangle.color = {.1f, .8f, .1f};
	triangle.transform2d.translation.x = .2f;
	triangle.transform2d.scale = { 2.f, .5f };
	triangle.transform2d.rotation = .25f * glm::two_pi<float>();

	gameObjects.push_back(std::move(triangle));
}

void Game::recreateSwapChain()
{
	auto extent = gameWindow.getExtent();

	while (extent.width == 0 || extent.height == 0)
	{
		extent = gameWindow.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(gameDevice.device());

	if (gameSwapChain == nullptr)
	{
		gameSwapChain = std::make_unique<B3DSwapChain>(gameDevice, extent);
	}
	else
	{
		gameSwapChain = std::make_unique<B3DSwapChain>(gameDevice, extent, std::move(gameSwapChain));

		if (gameSwapChain->imageCount() != commandBuffers.size())
		{
			freeCommandBuffers();
			createCommandBuffers();
		}
	}
	
	createPipeline();
}

void Game::recordCommandBuffer(int imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin command buffer recording!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = gameSwapChain->getRenderPass();
	renderPassInfo.framebuffer = gameSwapChain->getFrameBuffer(imageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = gameSwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(gameSwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(gameSwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{ {0, 0}, gameSwapChain->getSwapChainExtent() };

	vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

	renderGameObjects(commandBuffers[imageIndex]);

	vkCmdEndRenderPass(commandBuffers[imageIndex]);

	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to end command buffer recording!");
	}
}

void Game::renderGameObjects(VkCommandBuffer commandBuffer)
{
	gameRenderPipeline->bind(commandBuffer);

	for (auto& obj : gameObjects)
	{
		obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

		SimplePushConstantData push{};
		push.offset = obj.transform2d.translation;
		push.color = obj.color;
		push.transform = obj.transform2d.mat2();

		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
		obj.model->bind(commandBuffer);
		obj.model->draw(commandBuffer);
	}
}
