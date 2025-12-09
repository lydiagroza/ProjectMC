// ConsoleGame/main.cpp
#include <iostream>
#include "Game.h"
#include "testEngine.h" // Noua clasa Controller

int main() {
    std::cout << "=== 7 WONDERS DUEL - CONSOLE EDITION ===\n";

    try {
        Game game;

        // 1. Creează și pornește controlerul
        TestEngine runner(game);

        // 2. Rulează tot jocul
        runner.runGameLoop();

    }
    catch (const std::exception& e) {
        std::cerr << "CRASH: " << e.what() << std::endl;
    }

    std::cout << "Joc terminat. Apasa Enter.";
    std::cin.ignore();
    std::cin.get();
    return 0;
}