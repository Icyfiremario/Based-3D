#pragma once

#include "B3DWindow.h"
#include "B3DPipeline.h"
#include "B3DDevice.h"
#include "B3DSwapChain.h"
#include "B3DModel.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <array>
#include <stdexcept>


class Game
{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Game();
		~Game();

		Game(const Game &) = delete;
		Game &operator=(const Game &) = delete;

		void run();


	private:

		B3DWindow gameWindow{ WIDTH, HEIGHT, "Based Engine 3D" };
		B3DDevice gameDevice{ gameWindow };


		std::unique_ptr<B3DSwapChain> gameSwapChain;

		std::unique_ptr<B3DPipeline> gameRenderPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;

		std::unique_ptr<B3DModel> gameModel;

		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void loadModels();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
};