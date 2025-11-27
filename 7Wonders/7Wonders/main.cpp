#include <iostream>
#include <exception>
#include "Game.h" // Include-ul esențial pentru clasa Game

int main() {
    try {
     
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        // Prinde orice eroare standard care ar putea apărea.
        std::cerr << "[CRITICAL ERROR]: A aparut o eroare neasteptata: " << e.what() << std::endl;
        return 1; // Returnează un cod de eroare
    }
    catch (...) {
        // Prinde orice alt tip de eroare necunoscută.
        std::cerr << "[CRITICAL ERROR]: A aparut o eroare necunoscuta." << std::endl;
        return 2; // Returnează un alt cod de eroare
    }

    std::cout << "\nJocul s-a terminat cu succes. La revedere!\n";
    return 0; // Succes
}