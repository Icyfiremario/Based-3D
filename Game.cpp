#include "Game.h"
#include <iostream>



void Game::run()
{
	while (!gameWindow.shouldClose())
	{
		glfwPollEvents();
		//Game::update();
	}
}