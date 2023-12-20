#include "Game.h"
#include <iostream>

Game::Game()
{
	loadGameObjects();
}

Game::~Game()
{
}

void Game::run()
{
	SimpleRenderSystem simpleRenderSystem{ gameDevice, gameRenderer.getSwapChainRenderPass() };

	while (!gameWindow.shouldClose())
	{
		frameCnt++;
		glfwPollEvents();
		
		if (auto commandBuffer = gameRenderer.beginFrame())
		{
			gameRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
			gameRenderer.endSwapChainRenderPass(commandBuffer);
			gameRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(gameDevice.device());
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