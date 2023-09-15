#pragma once

#include "B3DWindow.h"
#include "B3DPipeline.h"
#include "B3DDevice.h"
#include "B3DSwapChain.h"

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
		B3DSwapChain gameSwapChain{ gameDevice, gameWindow.getExtent() };

		std::unique_ptr<B3DPipeline> gameRenderPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;

		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();
};