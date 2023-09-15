#pragma once

#include "B3DWindow.h"
#include "B3DPipeline.h"
#include "B3DDevice.h"
#include "B3DSwapChain.h"


class Game
{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();


	private:

		B3DWindow gameWindow{ WIDTH, HEIGHT, "Based Engine 3D" };
		B3DDevice gameDevice{ gameWindow };
		B3DSwapChain gameSwapChain{ gameDevice, gameWindow.getExtent() };
		B3DPipeline gameRenderPipeline{gameDevice, "simple_shader.vert.spv", "simple_shader.frag.spv", B3DPipeline::deafultPipelineConfigInfo(WIDTH, HEIGHT)};
};