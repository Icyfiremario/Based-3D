#pragma once

//Local
#include "B3DWindow.h"
#include "B3DDevice.h"
#include "B3DSwapChain.h"
#include "B3DGameObj.h"
#include "B3DRenderer.h"
#include "SimpleRenderSystem.h"
#include "B3DCamera.h"
#include "keyboardMovementController.h"
#include "B3DBuffer.h"

//GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//STD
#include <memory>
#include <vector>
#include <array>
#include <stdexcept>
#include <chrono>
#include <cmath>
#include <numeric>


class Game
{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		static constexpr int MAX_FRAME_TIME = 10;

		Game();
		~Game();

		Game(const Game &) = delete;
		Game &operator=(const Game &) = delete;

		void run();


	private:

		B3DWindow gameWindow{ WIDTH, HEIGHT, "Based Engine 3D" };
		B3DDevice gameDevice{ gameWindow };
		B3DRenderer gameRenderer{ gameWindow, gameDevice };

		std::vector<B3DGameObj> gameObjects;

		void loadGameObjects();
};