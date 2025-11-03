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
    linkCards(era);
}

void Board::linkCards(int eraIndex) {
    const auto& eraIndexes =
        (eraIndex == 1 ? eraChildIndexes1 :
            eraIndex == 2 ? eraChildIndexes2 :
            eraChildIndexes3);

    for (size_t r = 0; r + 1 < activeCards.size(); ++r) {
        const auto& row = activeCards[r];
        const auto& nextRow = activeCards[r + 1];

        for (size_t i = 0; i < row.size(); ++i) {
            CardNode* parent = row[i];

            // Verificăm că există definiție de copii pentru această carte
            if (i >= eraIndexes[r].size()) continue;

            // Adăugăm fiecare copil definit în vector
            for (int childIndex : eraIndexes[r][i]) {
                if (childIndex >= 0 && childIndex < static_cast<int>(nextRow.size())) {
                    parent->addChild(nextRow[childIndex]);
                }
                else {
                    // Optional: mesaj de warning dacă indexul e invalid
                    std::cerr << "WARN: Invalid child index " << childIndex
                        << " for parent at row " << r << " card " << i << "\n";
                }
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