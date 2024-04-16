#include <iostream>
#include <fstream>
#include <filesystem>
#include "include/common.h"
#include "include/GameEngine.h"

int main()
{
    try
    {
        GameEngine g("assets/assets.txt");
        std::cout << "Game initialized with MENU screen\n";
        std::cout << "Running the game now\n";
        g.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}