//Local
#include "Game.h"

//STD
#include <cstdlib>
#include <iostream>
#include <stdexcept>


//Plog
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

int main()
{

	plog::init(plog::debug, "./logs/runtime_log.log");

	PLOGI << "Starting engine";

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