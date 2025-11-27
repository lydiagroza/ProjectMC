#include "CardsPyramid.h"
#include "CardBase.h" 
#include <iostream>

void CardsPyramid::clear() {
    m_rows.clear();
}

void CardsPyramid::build(int age, std::vector<CardBase*>& deck) {
    clear();

    if (age < 1 || age > 3)
        return;

    int deckIndex = 0;
    int rowNumber = 0;

    // 1. Alegem layout-ul (câte cărți pe rând)
    const std::vector<int>& currentLayout = m_eraLayouts[age - 1];

    for (int rowSize : currentLayout) {
        std::vector<std::unique_ptr<CardNode>> row;
        row.reserve(rowSize);

        for (int i = 0; i < rowSize; ++i) {
            // Safety check
            if (deckIndex >= deck.size()) break;

            auto newNode = std::make_unique<CardNode>();
            newNode->setCard(deck[deckIndex]);

            // Logica FaceUp / FaceDown
            bool isFaceUp = false;
            isFaceUp = (rowNumber % 2 == 0);

            newNode->setFace(isFaceUp ? Face::Up : Face::Down);

            row.push_back(std::move(newNode));
            deckIndex++;
        }
        m_rows.push_back(std::move(row));
        rowNumber++;
    }

    // 2. Apelăm funcția de link cu numărul Erei
    linkCards(age);
}

void CardsPyramid::linkCards(int ageIndex)
{
    // Aici selectăm harta corectă folosind logica ta
    const std::vector<std::vector<std::vector<int>>>* ageIndexes = nullptr;

    if (ageIndex == 1) ageIndexes = &m_ageChildIndexes1;
    else if (ageIndex == 2) ageIndexes = &m_ageChildIndexes2;
    else ageIndexes = &m_ageChildIndexes3;

    // Parcurgem rândurile din m_rows (ATENTIE: foloseam m_rows, nu m_activeCards)
    for (size_t r = 0; r < m_rows.size() - 1; ++r) {

        // Verificăm să nu ieșim din harta de link-uri
        if (r >= ageIndexes->size()) break;

        const auto& linksForThisRow = (*ageIndexes)[r];

        for (size_t i = 0; i < m_rows[r].size(); ++i) {
            // Daca nu avem definitie pentru nodul i, sărim
            if (i >= linksForThisRow.size()) continue;

            // Luăm pointerul brut din unique_ptr cu .get()
            CardNode* parent = m_rows[r][i].get();

            // Iterăm prin copiii definiți în hartă
            for (int childIndex : linksForThisRow[i]) {
                // Verificăm dacă copilul există fizic
                if (childIndex >= 0 && childIndex < m_rows[r + 1].size()) {
                    CardNode* child = m_rows[r + 1][childIndex].get();

                    // Facem legătura
                    parent->addChild(child);
                }
                else {
                    // Debugging (Opțional)
                    // std::cerr << "WARN: Invalid child index " << childIndex << "\n";
                }
            }
        }
    }
}

// Trebuie să implementăm asta corect, altfel Board-ul nu poate accesa cărțile!
CardNode* CardsPyramid::getNodeAt(size_t row, size_t col) const
{
    if (row >= m_rows.size()) return nullptr;
    if (col >= m_rows[row].size()) return nullptr;

    return m_rows[row][col].get();
}

void CardsPyramid::updateVisibility() {
    for (const auto& row : m_rows) {
        for (const auto& nodePtr : row) {
            CardNode* node = nodePtr.get();

            // Verificăm dacă e pe masă și ascuns
            if (!node->isPlayed() && node->getFace() == Face::Down) {

                bool isBlocked = false;
                for (const auto* child : node->getChildren()) {
                    // Dacă un copil NU e jucat, înseamnă că blochează
                    if (!child->isPlayed()) {
                        isBlocked = true;
                        break;
                    }
                }

                if (!isBlocked) {
                    node->setFace(Face::Up);
                }
            }
        }
    }
}

bool CardsPyramid::isEmpty() const {
    for (const auto& row : m_rows) {
        for (const auto& nodePtr : row)
            if (!nodePtr->isPlayed())
                return false;
    }
    return true;
}