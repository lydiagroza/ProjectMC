#include "Board.h"

void Board::setupCards(int era, std::vector<CardNode*>& eraCards)
{
    activeCards.clear();

    const std::vector<int>& layout = eraLayouts[era - 1];
    int index = 0;
    int rowNumber = 0;

    // 1. Așezarea Cărților (Vârf -> Bază)
    for (int rowSize : layout) {
        std::vector<CardNode*> row;
        row.reserve(rowSize);
        for (int i = 0; i < rowSize; ++i) {
            row.push_back(eraCards[index++]);
            row.back()->setFace(rowNumber % 2 == 0 ? Face::Up : Face::Down);
        }
        rowNumber++;
        activeCards.push_back(std::move(row));
    }

    for (int r = static_cast<int>(activeCards.size()) - 2; r >= 0; --r)
    {

        std::vector<CardNode*>& blockerRow = activeCards[r];
        std::vector<CardNode*>& blockedRow = activeCards[r + 1];

        const size_t pSize = blockerRow.size();
        const size_t cSize = blockedRow.size();

        for (size_t i = 0; i < pSize; ++i) {

            // Cartea blocantă curentă: blockerCard
            CardNode* blockerCard = blockerRow[i];

            // Cazul 1: Blocantul acoperă cartea de pe poziția i din rândul de jos
            if (i < cSize) {
                CardNode* blockedCard = blockedRow[i];

                // Relația INVERSATĂ (Blocatul adaugă Blocantul ca "Child"):
                // Acum, blockedCard (Baza/Blocatul) va avea blockerCard (Vârful/Blocantul) ca "copil"
                blockedCard->addChild(blockerCard);
            }

            // Cazul 2: Blocantul acoperă cartea de pe poziția i+1 din rândul de jos
            if (i + 1 < cSize) {
                CardNode* blockedCard = blockedRow[i + 1];

                // Relația INVERSATĂ (Blocatul adaugă Blocantul ca "Child"):
                blockedCard->addChild(blockerCard);
            }
        }
    }
}
void Board::printBoard() const {
    int totalRows = static_cast<int>(activeCards.size());

    // 1. Găsim rândul cel mai lat pentru o centrare corectă
    size_t maxRowSize = 0;
    for (const auto& row : activeCards) {
        if (row.size() > maxRowSize) {
            maxRowSize = row.size();
        }
    }

    std::cout << "\n===== STRUCTURA PIRAMIDEI =====\n\n";

    // Iterăm de la rândul 0 (vârful) la ultimul rând (baza), conform output-ului tău
    for (int r = 0; r < totalRows; ++r) {
        const auto& row = activeCards[r];

        // 2. Calculul spațiilor pentru centrare
        // Diferența în număr de cărți * Lățimea unei cărți / 2
        // Presupunând că o carte ocupă 6 caractere ("[:U]  ")
        // O unitate de shift e 3 (jumătate din lățimea vizuală a unei cărți)
        int spaces = (maxRowSize - row.size()) * 3;

        // indentare
        for (int s = 0; s < spaces; ++s)
            std::cout << ' ';

        // afișăm toate cărțile de pe rând
        for (const auto* card : row) {
            // Poți adăuga aici logica de afișare a statusului de jucabilitate
            // (dacă lista de copii/blocanți e goală), ca în sugestia anterioară.
            std::cout << "[" << card->getName()
                << ":" << (card->getFace() == Face::Up ? "U" : "D") << "]  ";
        }
        std::cout << "\n\n";
    }

    std::cout << "===============================\n";
}

void Board::printChildrenList() const {
    std::cout << "\n===== VERIFICARE RELAȚII BLOCARE (BLOCAT -> BLOCANT) =====\n";
    std::cout << " (Copiii sunt cărțile care blochează)\n\n";

    // activeCards conține rândurile de cărți de la vârf (r=0) la bază.
    for (const auto& row : activeCards) {
        for (const auto* card : row) {

            // Afișăm cartea curentă
            std::cout << "-> " << card->getName()
                << " (Față: " << (card->getFace() == Face::Up ? "U" : "D") << ")";

            // Afișăm "copiii" (adică blocanții)
            const auto& children = card->getChildren(); // Presupunem că ai o metodă getChildren() care returnează lista de CardNode*

            if (children.empty()) {
                std::cout << " | COPII (BLOCANȚI): NICIUNUL [ACCESIBIL]\n";
            }
            else {
                std::cout << " | COPII (BLOCANȚI): ";
                for (const auto* blocker : children) {
                    std::cout << blocker->getName() << " ";
                }
                std::cout << "\n";
            }
        }
    }

    std::cout << "\n=======================================================\n";
}