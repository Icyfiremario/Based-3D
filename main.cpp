#include "Game.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
	Game game{};

	try 
	{
		game.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}