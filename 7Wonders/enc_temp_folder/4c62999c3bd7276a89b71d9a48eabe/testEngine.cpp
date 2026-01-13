#include "testEngine.h"
#include <iostream>
#include <limits> // Required for numeric_limits

// Constructor (Doar inițializează referința)
TestEngine::TestEngine(Game& game) : m_game(game) {
    // Aici inițializăm Game-ul
    m_game.initialize();
}

void TestEngine::runWonderDraftLoop() {
    std::cout << "\n--- FAZA DE SELECTIE A MINUNILOR ---\n";
    while (m_game.getDraftPhase() > 0 && !m_game.getCurrentDraftSet().empty()) {
        const auto& availableWonders = m_game.getCurrentDraftSet();
        Player* currentPlayer = m_game.getCurrentPlayer();

        std::cout << "\nEste randul lui " << currentPlayer->getName() << " sa aleaga o minune.\n";
        std::cout << "Minuni disponibile:\n";

        for (size_t i = 0; i < availableWonders.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << availableWonders[i]->getName() << " (ID: " << availableWonders[i]->getId() << ")\n";
        }

        int choice = -1;
        bool validChoice = false;

        while (!validChoice) {
            std::cout << "Alege numarul minunii (1-" << availableWonders.size() << "): ";
            std::cin >> choice;

            if (std::cin.fail() || choice < 1 || choice > static_cast<int>(availableWonders.size())) {
                std::cout << "Selectie invalida. Te rog incearca din nou.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                // --- FIX ---
                // 1. Get the ID and name *before* modifying the vector.
                int wonderId = availableWonders[choice - 1]->getId();
                std::string wonderName = availableWonders[choice - 1]->getName();

                // 2. Attempt to draft the wonder.
                if (m_game.draftWonder(wonderId)) {
                    std::cout << currentPlayer->getName() << " a ales " << wonderName << ".\n";
                    validChoice = true; // Exit the input loop for this turn
                } else {
                    // This case should not happen if input is valid, but it's a good safeguard.
                    std::cout << "Eroare la draft. Minunea nu a putut fi selectata.\n";
                }
            }
        }
    }
    std::cout << "\n--- SELECTIA MINUNILOR S-A INCHEIAT ---\n";
}


void TestEngine::runGameLoop() {
    // --- NEW: Run the wonder draft first ---
    runWonderDraftLoop();

    // This is the existing main game loop
    int turnCounter = 1;
    while (!m_game.isGameOver()) {
        std::cout << "\n\n################ TURNA " << turnCounter << " ################\n";

        // 1. Afișează starea
        m_game.printGameState();

        // 2. Execută tura (aici se citește input și se schimbă tura intern)
        m_game.handlePlayerAction();

        // 3. Verifică sfârșitul Epocii
        if (m_game.isEndOfAge()) {
            m_game.startNextAge();
        }

        turnCounter++;

        // Oprire de siguranță
        if (turnCounter > 50) {
            std::cout << "Limita de ture atinsa (Debug).\n";
            break;
        }
    }
}