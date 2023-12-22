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
    B3DCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = B3DGameObj::createGameObject();
    keyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

	while (!gameWindow.shouldClose())
	{
		glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        frameTime = fmin(frameTime, MAX_FRAME_TIME);

        cameraController.moveInPlaneXZ(gameWindow.getGLFWwindow(), frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = gameRenderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
		
		if (auto commandBuffer = gameRenderer.beginFrame())
		{
			gameRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
			gameRenderer.endSwapChainRenderPass(commandBuffer);
			gameRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(gameDevice.device());
}

void Game::loadGameObjects()
{
    std::shared_ptr<B3DModel> cubeModel = B3DModel::createModelFromFile(gameDevice, "sphere.wobj");

    auto cube = B3DGameObj::createGameObject();
    cube.model = cubeModel;
    cube.transform.translation = {.0f, .0f, 2.5f};
    cube.transform.scale = { .5f, .5f, .5f };

    gameObjects.push_back(std::move(cube));
}