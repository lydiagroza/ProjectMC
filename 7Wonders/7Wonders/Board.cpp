#include "Board.h"
#include "CardBase.h"
#include <iomanip>   // pentru std::setw

void Board::setupCards(int era, std::vector<CardBase*>& deck)
{
    // Curatam tabla veche (daca exista)
    // NOTA: Aici ar trebui stersi pointerii vechi daca nu folosesti smart pointers, 
    // pentru a evita memory leaks. Dar ne focusam pe logica acum.
    m_activeCards.clear();

    // Verificam sa nu accesam un layout inexistent
    if (era < 1 || era > 3) return;

    const std::vector<int>& layout = m_eraLayouts[era - 1];
    int deckIndex = 0;
    int rowNumber = 0;

    // 1. Așezarea Cărților
    for (int rowSize : layout) {
        std::vector<CardNode*> row;
        row.reserve(rowSize);

        for (int i = 0; i < rowSize; ++i) {
            if (deckIndex >= deck.size())
                break;
            CardNode* newNode = new CardNode();
            newNode->setCard(deck[deckIndex]);
            bool isFaceUp = false;
            if (era == 1 || era == 3) 
                isFaceUp = (rowNumber % 2 == 0);
            else if (era == 2)
                isFaceUp = (rowNumber % 2 != 0);

            newNode->setFace(isFaceUp ? Face::Up : Face::Down);

            row.push_back(newNode);
            deckIndex++;
        }
        m_activeCards.push_back(std::move(row));
        rowNumber++;
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

void Board::setAvailableProgressTokens(const std::vector<std::shared_ptr<ProgressToken>>& tokens)
{
    m_availableProgressTokens = tokens;
}

std::vector<std::shared_ptr<ProgressToken>> Board::getAvailableProgressTokens() const
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
            << t->getName() << "\n";
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
void Board::addCardToDiscardPile(CardBase* card)
{
    if (card) {
        m_discardPile.push_back(card);
    }
}

const std::vector<CardBase*>& Board::getDiscardPile() const
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
    for (const auto* card : m_discardPile)
        std::cout << card->get_name() << ", ";

    std::cout << "]\n";
}

void Board::updateVisibility()
{
    for (auto& row : m_activeCards) {
        for (auto* node : row) {
            if (!node->isPlayed() && node->getFace() == Face::Down) {

          
                bool isBlocked = false;
                const auto& children = node->getChildren();

                for (const auto* child : children) 
                    if (!child->isPlayed()) {
                        isBlocked = true;
                        break;
                    }
                if (!isBlocked) 
                    node->setFace(Face::Up);
         
            }
        }
    }
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


bool Board::isPyramidEmpty() const {
    for (const auto& row : m_activeCards) {
        for (const auto* node : row) {
            // Daca gasim macar o carte care NU a fost luata, piramida nu e goala
            if (!node->isPlayable()) {
                return false;
            }
        }
    }
    return true;
}