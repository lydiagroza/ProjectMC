#include "Board.h"

void Board::setupCards(int era, std::vector<CardNode*>& eraCards)
{
    activeCards.clear();
    const std::vector<int>& layout = eraLayouts[era - 1]; //layout pt epoca curenta
    int index = 0;
    int rowNumber = 0;
    for (int rowSize : layout) {
        std::vector<CardNode*> row;
        for (int i = 0; i < rowSize; ++i) {
            row.push_back(eraCards[index++]);
            row.back()->setFace(rowNumber % 2 == 0 ? Face::Up : Face::Down);
        }
		rowNumber++;
        activeCards.push_back(row);
    }
    for (int r = activeCards.size() - 2; r >= 0; --r)
    {
        std::vector<CardNode*>& parentRow = activeCards[r];
        std::vector<CardNode*>& childRow = activeCards[r + 1];

        for (int i = 0; i < parentRow.size(); ++i) {
            parentRow[i]->addChild(childRow[i]);
            if (i + 1 < childRow.size()) {
                parentRow[i]->addChild(childRow[i + 1]);
            }
        }
    }
}
void Board::printBoard() const {
    int totalRows = activeCards.size();

    for (int r = totalRows - 1; r >= 0; --r) { // de sus (vârful) în jos (baza)
        const auto& row = activeCards[r];

        // indentare pentru centru
        int spaces = totalRows - r - 1;
        for (int s = 0; s < spaces; ++s) std::cout << "  ";

        for (const auto* card : row) {
            std::cout << card->getName()
                << "(" << (card->getFace() == Face::Up ? "U" : "D") << ") ";
        }
        std::cout << "\n";
    }
}


