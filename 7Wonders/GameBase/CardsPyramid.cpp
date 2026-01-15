#include "CardsPyramid.h"
#include "CardBase.h" 
#include <iostream>

void CardsPyramid::clear() {
    m_rows.clear();
}

void CardsPyramid::build(int age, std::vector<std::shared_ptr<CardBase>>& deck) {
    clear();


    if (age < 1 || age > 3)
        return;

    int deckIndex = 0;
    int rowNumber = 0;

    const std::vector<int>& currentLayout = m_ageLayouts[age - 1];

    for (int rowSize : currentLayout) {
        std::vector<std::unique_ptr<CardNode>> row;
        row.reserve(rowSize);

        for (int i = 0; i < rowSize; ++i) {
            if (deckIndex >= (int)deck.size()) 
                break;

            auto newNode = std::make_unique<CardNode>();
            newNode->setCard(deck[deckIndex]);

            bool isFaceUp = false;
            isFaceUp = (rowNumber % 2 == 0);

            newNode->setFace(isFaceUp ? Face::Up : Face::Down);

            row.push_back(std::move(newNode));
            deckIndex++;
        }
        m_rows.push_back(std::move(row));
        rowNumber++;
    }

    linkCards(age);
}

void CardsPyramid::linkCards(int ageIndex)
{
    const std::vector<std::vector<std::vector<int>>>* ageIndexes = nullptr;

    if (ageIndex == 1) ageIndexes = &m_ageChildIndexes1;
    else if (ageIndex == 2) ageIndexes = &m_ageChildIndexes2;
    else ageIndexes = &m_ageChildIndexes3;

    for (size_t r = 0; r < m_rows.size() - 1; ++r) {

        if (r >= ageIndexes->size()) 
            break;

        const auto& linksForThisRow = (*ageIndexes)[r];

        for (size_t i = 0; i < m_rows[r].size(); ++i) {
            if (i >= linksForThisRow.size()) 
                continue;

            CardNode* parent = m_rows[r][i].get();

            for (int childIndex : linksForThisRow[i])
                if (childIndex >= 0 && childIndex < (int)m_rows[r + 1].size()) 
                {
                    CardNode* child = m_rows[r + 1][childIndex].get();
                    parent->addChild(child);
                }       
        }
    }
}

CardNode* CardsPyramid::getNodeAt(size_t row, size_t col) const
{
    if (row >= m_rows.size()) return nullptr;
    if (col >= m_rows[row].size()) return nullptr;

    return m_rows[row][col].get();
}

void CardsPyramid::updateVisibility() 
{
    for (const auto& row : m_rows)
    {
        for (const auto& nodePtr : row) 
        {
            CardNode* node = nodePtr.get();
            if (!node->isPlayed() && node->getFace() == Face::Down) 
            {
                bool isBlocked = false;
                for (const auto* child : node->getChildren())
                    if (!child->isPlayed()) 
                    {
                        isBlocked = true;
                        break;
                    }
                if (!isBlocked)
                    node->setFace(Face::Up);
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

const std::vector<std::vector<std::unique_ptr<CardNode>>>& CardsPyramid::getRows() const
{
    return m_rows;
}
