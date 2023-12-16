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
	createPipeline();
}

Game::~Game()
{
	vkDestroyPipelineLayout(gameDevice.device(), pipelineLayout, nullptr);
}

void Game::run()
{
	while (!gameWindow.shouldClose())
	{
		frameCnt++;
		glfwPollEvents();
		
		if (auto commandBuffer = gameRenderer.beginFrame())
		{
			gameRenderer.beginSwapChainRenderPass(commandBuffer);
			renderGameObjects(commandBuffer);
			gameRenderer.endSwapChainRenderPass(commandBuffer);
			gameRenderer.endFrame();
		}
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
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};

	B3DPipeline::deafultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = gameRenderer.getSwapChainRenderPass();
	pipelineConfig.pipelineLayout = pipelineLayout;

	gameRenderPipeline = std::make_unique<B3DPipeline>(gameDevice, "simple_shader.vert.spv", "simple_shader.frag.spv", pipelineConfig);
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
