#include "Game.h"
#include <iostream>

struct GlobalUbo
{
    glm::mat4 projectionView{ 1.f };
    glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
};

Game::Game()
{
    globalPool = B3DDescriptorPool::Builder(gameDevice).setMaxSets(B3DSwapChain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, B3DSwapChain::MAX_FRAMES_IN_FLIGHT).build();
	loadGameObjects();
}

Game::~Game()
{
}

void Game::run()
{
    std::vector<std::unique_ptr<B3DBuffer>> ubobuffers(B3DSwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < ubobuffers.size(); i++)
    {
        ubobuffers[i] = std::make_unique<B3DBuffer>(gameDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        ubobuffers[i]->map();
    }

    auto globalSetLayout = B3DDescriptorSetLayout::Builder(gameDevice).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT).build();

    std::vector<VkDescriptorSet> globalDescriptorSets(B3DSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++)
    {
        auto bufferInfo = ubobuffers[i]->descriptorInfo();
        B3DDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
    }

	SimpleRenderSystem simpleRenderSystem{ gameDevice, gameRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    B3DCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = B3DGameObj::createGameObject();
    keyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    PLOGI << "Game loop started";

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
            int frameIndex = gameRenderer.getFrameIndex();
            FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

            //Update
            GlobalUbo ubo{};
            ubo.projectionView = camera.getProjection() * camera.getView();
            ubobuffers[frameIndex]->writeToBuffer(&ubo);
            ubobuffers[frameIndex]->flush();

            //Render
			gameRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
			gameRenderer.endSwapChainRenderPass(commandBuffer);
			gameRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(gameDevice.device());
}

void Game::loadGameObjects()
{
    PLOGI << "Loading 3D models";

    std::shared_ptr<B3DModel> smoothSphereModel = B3DModel::createModelFromFile(gameDevice, "smooth_sphere.wobj");

    auto smoothSphere = B3DGameObj::createGameObject();
    smoothSphere.model = smoothSphereModel;
    smoothSphere.transform.translation = {.0f, .0f, 2.5f};
    smoothSphere.transform.scale = { .5f, .5f, .5f };

    gameObjects.push_back(std::move(smoothSphere));
}