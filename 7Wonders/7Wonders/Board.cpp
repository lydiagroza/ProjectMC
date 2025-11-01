#include "Board.h"

void Board::setupCards(int era, std::vector<CardNode*>& eraCards)
{
    activeCards.clear();
    const std::vector<int>& layout = eraLayouts[era - 1]; //layout pt epoca curenta
    int index = 0;
    int rowNumber = 0;
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

    // NOUA LOGICĂ: Legăm Blocata la Blocant (care devine Copil)
    // Parcurgem rândurile de la penultimul în sus (de jos în sus)
    for (int r = static_cast<int>(activeCards.size()) - 2; r >= 0; --r)
    {
        // parentRow este rândul superior (Blocantul)
        std::vector<CardNode*>& blockerRow = activeCards[r];
        // childRow este rândul inferior (Blocatul)
        std::vector<CardNode*>& blockedRow = activeCards[r + 1];

        const size_t pSize = blockerRow.size();
        const size_t cSize = blockedRow.size();

        for (size_t i = 0; i < pSize; ++i) {

            // Cartea Blocată este în rândul inferior (r+1), la poziția i
            // Cartea Blocantă este în rândul superior (r), la poziția i
            if (i < cSize) {
                // Legătura inversă: Blocata adaugă Blocantul ca "Copil"
                // childRow[i] (Blocata) este jucabilă doar când acest "Copil" (Blocantul) e eliminat.
                blockedRow[i]->addChild(blockerRow[i]);
            }

            // Cartea Blocată este în rândul inferior (r+1), la poziția i+1
            // Cartea Blocantă este în rândul superior (r), la poziția i
            if (i + 1 < cSize) {
                // Legătura inversă: Blocata adaugă Blocantul ca "Copil"
                blockedRow[i + 1]->addChild(blockerRow[i]);
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