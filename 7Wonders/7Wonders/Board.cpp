#include "Board.h"
#include <iomanip>   // pentru std::setw

void Board::setupCards(int era, std::vector<CardNode*>& eraCards)
{
    m_activeCards.clear();

    const std::vector<int>& layout = m_eraLayouts[era - 1];
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
        m_activeCards.push_back(std::move(row));
    }
    linkCards(era);
}

void Board::linkCards(int eraIndex)
{
    const auto& eraIndexes =
        (eraIndex == 1 ? m_eraChildIndexes1 :
            eraIndex == 2 ? m_eraChildIndexes2 :
            m_eraChildIndexes3);

    for (size_t r = 0; r + 1 < m_activeCards.size(); ++r) {
        const auto& row = m_activeCards[r];
        const auto& nextRow = m_activeCards[r + 1];

        for (size_t i = 0; i < row.size(); ++i) {
            CardNode* parent = row[i];

            if (i >= eraIndexes[r].size()) continue;

            for (int childIndex : eraIndexes[r][i]) {
                if (childIndex >= 0 && childIndex < static_cast<int>(nextRow.size())) {
                    parent->addChild(nextRow[childIndex]);
                }
                else {
                    std::cerr << "WARN: Invalid child index " << childIndex
                        << " for parent at row " << r << " card " << i << "\n";
                }
            }
        }
    }
}

void Board::setAvailableProgressTokens(const std::vector<ProgressToken>& tokens)
{
    m_availableProgressTokens = tokens;
}

std::vector<ProgressToken> Board::getAvailableProgressTokens() const
{
    return m_availableProgressTokens;
}


//tokenii aia verzi
void Board::printTokens(std::ostream& fout) const
{
    if (m_availableProgressTokens.empty()) {
        fout << "Nu exista tokeni vizibili pe tabla.\n";
        return;
    }

    fout << "=== Progress Tokens Vizibile ===\n";

    for (size_t i = 0; i < m_availableProgressTokens.size(); ++i) {
        const auto& t = m_availableProgressTokens[i];
        fout << std::setw(2) << (i + 1) << ") "
            << t.name << " — " << t.description << "\n";
    }
}

//tabla militara

void Board::printMilitaryTrack(std::ostream& fout) const
{

	int currentPos = m_militaryTrack.getPawnPosition();
    std::cout << "\n[Military]: P2 ";

    for (int i = -9; i <= 9; ++i)
    {
    
        if (i == currentPos) {
            std::cout << "(X)";
        }
        else if (i == -6) {
            if (m_militaryTrack.isTokenVisible(0)) std::cout << "[5]";
            else std::cout << " . "; // Tokenul a fost luat
        }
        else if (i == -3) {
            if (m_militaryTrack.isTokenVisible(1)) std::cout << "[2]";
            else std::cout << " . ";
        }
        else if (i == 3) {
            if (m_militaryTrack.isTokenVisible(2)) std::cout << "[2]";
            else std::cout << " . ";
        }
        else if (i == 6) {
            if (m_militaryTrack.isTokenVisible(3)) std::cout << "[5]";
            else std::cout << " . ";
        }
        else if (i == 0) {
            std::cout << " | ";
        }
        else {
            std::cout << " . ";
        }
    }

    std::cout << " P1\n";

}
//carti "arse"
void Board::addCardToDiscardPile(CardNode* card)
{
    if(card)
		m_discardPile.push_back(card);
}


const std::vector<CardNode*>& Board::getDiscardPile() const
{
    return m_discardPile;
}

void Board::printDiscardPile(std::ostream& fout) const
{
	fout << "\n=== Discard Pile ===\n";
    if(m_discardPile.empty()) {
        fout << "Nu exista carti in discard pile.\n";
        return;
    }
    std::cout << "[ ";
    for (const auto* card : m_discardPile) {
        // Presupunem că CardNode are metoda getName()
        std::cout << card->getName() << ", ";
    }
    std::cout << "]\n";
}


//arbore de carti

void Board::printCardsTree() const
{
    int totalRows = static_cast<int>(m_activeCards.size());

    size_t maxRowSize = 0;
    for (const auto& row : m_activeCards)
        if (row.size() > maxRowSize)
            maxRowSize = row.size();

    std::cout << "\n===== STRUCTURA PIRAMIDEI =====\n\n";

    for (int r = 0; r < totalRows; ++r) {
        const auto& row = m_activeCards[r];

        int spaces = (maxRowSize - row.size()) * 3;

        for (int s = 0; s < spaces; ++s)
            std::cout << ' ';

        for (const auto* card : row) {
            std::cout << "[" << card->getName()
                << ":" << (card->getFace() == Face::Up ? "U" : "D") << "]  ";
        }
        std::cout << "\n\n";
    }

    std::cout << "===============================\n";
}

void Board::printChildrenList() const
{
    std::cout << "\n===== VERIFICARE RELAȚII BLOCARE (BLOCAT -> BLOCANT) =====\n";
    std::cout << " (Copiii sunt cărțile care blochează)\n\n";

    for (const auto& row : m_activeCards) {
        for (const auto* card : row) {

            std::cout << "-> " << card->getName()
                << " (Față: "
                << (card->getFace() == Face::Up ? "U" : "D")
                << ")";

            const auto& children = card->getChildren();

            if (children.empty()) {
                std::cout << " | COPII (BLOCANȚI): NICIUNUL [ACCESIBIL]\n";
            }
            else {
                std::cout << " | COPII (BLOCANȚI): ";
                for (const auto* blocker : children)
                    std::cout << blocker->getName() << " ";
                std::cout << "\n";
            }
        }
    }

    std::cout << "\n=======================================================\n";
}
