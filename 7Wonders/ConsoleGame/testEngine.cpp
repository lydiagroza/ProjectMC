#include "testEngine.h"
#include <iostream>
#include <limits> 


TestEngine::TestEngine(Game& game) : m_game(game) {
    m_game.initialize();
}

void TestEngine::runWonderDraftLoop() {
    std::cout << "\n--- WONDER SELECTION PHASE ---\n";
    while (m_game.getDraftPhase() > 0 && !m_game.getCurrentDraftSet().empty()) {
        const auto& availableWonders = m_game.getCurrentDraftSet();
        Player* currentPlayer = m_game.getCurrentPlayer();

        std::cout << "\nIt's " << currentPlayer->getName() << "'s turn to choose a wonder.\n";
        std::cout << "Available wonders:\n";

        for (size_t i = 0; i < availableWonders.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << availableWonders[i]->getName() << " (ID: " << availableWonders[i]->getId() << ")\n";
        }

        int choice = -1;
        bool validChoice = false;

        while (!validChoice) {
            std::cout << "Choose the wonder number (1-" << availableWonders.size() << "): ";
            std::cin >> choice;

            if (std::cin.eof()) {
                 std::cerr << "Input stream ended. Exiting.\n";
                 exit(0);
            }

            if (std::cin.fail() || choice < 1 || choice > static_cast<int>(availableWonders.size())) {
                std::cout << "Invalid selection. Please try again.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
               
                int wonderId = availableWonders[choice - 1]->getId();
                std::string wonderName = availableWonders[choice - 1]->getName();

                if (m_game.draftWonder(wonderId)) {
                    std::cout << currentPlayer->getName() << " chose " << wonderName << ".\n";
                    validChoice = true; 
                } else {
                    std::cout << "Draft error. Wonder could not be selected.\n";
                }
            }
        }
    }
    std::cout << "\n--- WONDER SELECTION ENDED ---\n";
}


void TestEngine::runGameLoop() {
    runWonderDraftLoop();

    int turnCounter = 1;
    while (!m_game.isGameOver()) {
        std::cout << "\n\n################ TURN " << turnCounter << " ################\n";

        m_game.printGameState();

        m_game.handlePlayerAction();

        if (m_game.isEndOfAge()) {
            m_game.startNextAge();
        }

        turnCounter++;
        if (turnCounter > 70) {
            std::cout << "Turn limit reached (Debug).\n";
            break;
        }
    }
}