#include "Board.h"
#include "CardBase.h"
#include <iomanip>   // Asigură-te că ai acest include
#include <iostream>
#include <algorithm> // Pentru std::max

void Board::setupCards(int era, std::vector<CardBase*>& deck)
{
    m_activeCards.clear();
    if (era < 1 || era > 3) 
        return;

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
void Board::printTokens(std::ostream& os) const
{
    if (m_availableProgressTokens.empty()) {
        os << "No available progress tokens." << std::endl;
        return;
    }

    for (const auto& tokenPtr : m_availableProgressTokens) {
        if (tokenPtr) {
            os << *tokenPtr << "\n"; // Use the stream operator
        }
    }
    os << std::endl;
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
void Board::printCardsTree(std::ostream& os) const
{
    if (m_activeCards.empty()) {
        os << "Piramida este goala.\n";
        return;
    }

	//determin latimea maxima a unei carti pentru aliniere
    size_t max_card_width = 0;
    for (const auto& row : m_activeCards) {
        for (const auto* node : row) {
            std::string status = (node->isPlayable() ? "[OK]" : (node->getFace() == Face::Up ? "[U]" : "[D]"));
            size_t current_width = node->getName().length() + status.length() + 3;
            if (current_width > max_card_width) {
                max_card_width = current_width;
            }
        }
    }

	//latimea totala a piramidei
    size_t max_row_count = 0;
    for (const auto& row : m_activeCards) {
        if (row.size() > max_row_count) {
            max_row_count = row.size();
        }
    }
    const size_t total_width = max_row_count * (max_card_width + 2);

    //afisez cartile de pe fiecare rand centrat
    for (const auto& row : m_activeCards) {
        size_t current_row_width = row.size() * (max_card_width + 2);
        size_t padding = (total_width - current_row_width) / 2;
        
        os << std::string(padding, ' ');

        for (const auto* node : row) {
            if (node->isPlayed()) {
                os << std::string(max_card_width, ' ') << "  ";
            } else {
              
                std::string status = (node->isPlayable() ? "[OK]" : (node->getFace() == Face::Up ? "[U]" : "[D]"));
                os << std::left << std::setw(max_card_width) << (node->getName() + " " + status) << "  ";
            }
        }
        os << "\n\n";
    }

    os << "Legenda: [OK] = Jucabil, [U] = Cu fata in sus (blocat), [D] = Cu fata in jos\n";
    os << "===============================\n";
}

void Board::printChildrenList() const
{
    std::cout << "\nSe afiseaza relatiile de rudenie/blocare\n";
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
        for (const auto* node : row)
            if (!node->isPlayable())
                return false;
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, const Board& board)
{
    os << "========== GAME BOARD STATE ==========\n\n";

    os << "--- Card Pyramid ---\n";
    board.printCardsTree(os);
    os << "\n";

    os << "--- Military Track ---\n";
    board.printMilitaryTrack(os); 
    os << "\n";

    os << "--- Available Progress Tokens ---\n";
    board.printTokens(os); 
    os << "\n";

    os << "======================================\n";

    return os; 
}