#include <iostream>
#include <exception>
#include "Game.h"

int main() {
    try {
        Game game;
        
        // Call the existing and new print functions
        game.getSetup().printDecks();
        game.getSetup().printWonders(); 

        //game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "[CRITICAL ERROR]: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "[CRITICAL ERROR]: Unknown exception occurred." << std::endl;
        return -2;
    }
    return 0;
}