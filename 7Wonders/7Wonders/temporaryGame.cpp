#include <iostream>
#include <exception>
#include "GameSetup.h"
#include "Board.h"

int main() {
    try {
        std::cout << "============================================\n";
        std::cout << "   TEST INTEGRAR 7 WONDERS DUEL (MODERN C++)\n";
        std::cout << "============================================\n\n";

        // ---------------------------------------------------------
        // PASUL 1: Initializare (Loader -> Shuffle -> Setup Age I)
        // ---------------------------------------------------------
        std::cout << "[INFO] Se initializeaza GameSetup (Incarcare resurse...)\n";

        // Aici se intampla toata magia:
        // 1. Se citesc CSV-urile (AgeI, AgeII, AgeIII, Tokens)
        // 2. Se aleg 5 tokeni random
        // 3. Se pregatesc pachetele (se scot 3 carti random)
        // 4. Se construieste piramida pentru Age I
        GameSetup game;
		game.startAge(1);

        std::cout << "[OK] GameSetup initializat cu succes!\n\n";

        // Luam referinta la tabla pentru a verifica starea
        Board& board = game.getBoard();

        // ---------------------------------------------------------
        // PASUL 2: Verificare Tokeni Progres
        // ---------------------------------------------------------
        // Ar trebui sa vezi 5 nume de tokeni (ex: AGRICULTURE, LAW...)
        std::cout << "--- 1. TOKENI PROGRES EXTRASI ---\n";
        board.printTokens(std::cout);
        std::cout << "\n";

        // ---------------------------------------------------------
        // PASUL 3: Verificare Piramida Age I
        // ---------------------------------------------------------
        // Ar trebui sa vezi structura piramidei cu carti reale (Lumber Yard etc.)
        // Unele Face Up (U), altele Face Down (D)
        std::cout << "--- 2. PIRAMIDA AGE I ---\n";
        board.printCardsTree();
        std::cout << "\n";

        // ---------------------------------------------------------
        // PASUL 4: Verificare Discard Pile (Initial Gol)
        // ---------------------------------------------------------
        std::cout << "--- 3. DISCARD PILE (Initial) ---\n";
        board.printDiscardPile(std::cout);

        // ---------------------------------------------------------
        // PASUL 5: Testare Tranzitie Era (Age II)
        // ---------------------------------------------------------
        std::cout << "\n[INFO] Simulam trecerea la Age II...\n";
        game.startAge(2);

        std::cout << "--- 4. PIRAMIDA AGE II (Inversa) ---\n";
        board.printCardsTree();

    }
    catch (const std::exception& e) {
        // Prindem orice eroare (ex: fisier CSV lipsa, stoi error etc.)
        std::cerr << "\n[CRITICAL ERROR]: " << e.what() << "\n";
        std::cerr << "Verifica daca fisierele .csv sunt in folderul corect!\n";
    }

    return 0;
}