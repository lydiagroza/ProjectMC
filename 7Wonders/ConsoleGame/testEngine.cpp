#include "testEngine.h"
#include <iostream>

// Constructor (Doar inițializează referința)
TestEngine::TestEngine(Game& game) : m_game(game) {
    // Aici inițializăm Game-ul
    m_game.initialize();
}

void TestEngine::runGameLoop() {
    // Nu mai folosim variabila gameOver din main, ci m_game.m_gameOver (dacă este publică)
    // Sau, mai curat, folosim o funcție: m_game.isGameOver()

    int turnCounter = 1;

    // Mutăm bucla de joc din main() aici:
    while (!m_game.isGameOver()) {
        std::cout << "\n\n################ TURNA " << turnCounter << " ################\n";

        // --- AICI NU MAI AI NEVOIE DE LOGICA ACTVEPLAYER/OPPONENT ---
        // (Pentru că Game::handlePlayerAction și Game::printGameState accesează
        // deja m_game.m_currentPlayer și m_game.m_opponent)

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