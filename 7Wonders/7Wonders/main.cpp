#include <iostream>
#include <exception>
#include "Game.h"

int main() {
    try {

        Game game;
        //game.run();
    }
    catch (const std::exception& e) {
        // Prindem orice eroare critic? (ex: fi?iere lips?)
        std::cerr << "[CRITICAL ERROR]: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "[CRITICAL ERROR]: Unknown exception occurred." << std::endl;
        return -2;
    }
    return 0;
}